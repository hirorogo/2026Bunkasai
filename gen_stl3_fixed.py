#!/usr/bin/env python3
"""Generate gun STL: single manifold hollow body with barrel + tapered grip."""
import math

OUT = '/home/medicon-paki/work/2026Bunkasai/2026Bunkasai_gun.stl'

SECTIONS = 24

# Z-sections along the full length 0..155
# 0: barrel bottom
# 100: grip start (barrel outer=40, inner=36; grip outer=18, inner=15)
# 120: barrel end / grip continues
# 155: grip end

# Outer radii at key Z
Z_BARREL_START = 0.0
Z_GRIP_START   = 100.0
Z_BARREL_END   = 120.0
Z_GRIP_END     = 155.0

# Outer profile: (z, r_out)
OUTER_PROFILE = [
    (Z_BARREL_START, 40.0),   # barrel bottom
    (Z_GRIP_START,   40.0),   # barrel top / grip base (step down)
    (Z_GRIP_START,   18.0),   # grip outer start
    (Z_GRIP_END,     15.0),   # grip outer end (tapered)
]

# Inner profile: (z, r_in)
INNER_PROFILE = [
    (Z_BARREL_START, 36.0),   # barrel bottom
    (Z_GRIP_START,   36.0),   # barrel top / grip base (step down)
    (Z_GRIP_START,   15.0),   # grip inner start
    (Z_GRIP_END,     12.0),   # grip inner end (tapered)
]

verts = []
faces = []

def vi(x, y, z):
    verts.append((x, y, z))
    return len(verts) - 1

def ring(z, r):
    """Ring of vertices at (z, r), CCW from +X."""
    return [vi(r * math.cos(2*math.pi*i/SECTIONS),
              r * math.sin(2*math.pi*i/SECTIONS), z)
            for i in range(SECTIONS)]

def add_quad(a, b, c, d):
    """Emit two triangles for quad a-b-c-d (CCW = outward)."""
    faces.append([a, b, c])
    faces.append([a, c, d])

# ============================================================
# Build rings at each profile point
# ============================================================
outer_rings = [ring(z, r) for z, r in OUTER_PROFILE]
inner_rings = [ring(z, r) for z, r in INNER_PROFILE]

# ============================================================
# Outer wall (continuous, outward normals)
# ============================================================
for seg in range(len(outer_rings) - 1):
    r0 = outer_rings[seg]
    r1 = outer_rings[seg + 1]
    for i in range(SECTIONS):
        a, b = r0[i], r0[(i+1)%SECTIONS]
        c, d = r1[(i+1)%SECTIONS], r1[i]
        add_quad(a, b, c, d)

# ============================================================
# Inner wall (continuous, inward normals) — reverse winding
# ============================================================
for seg in range(len(inner_rings) - 1):
    r0 = inner_rings[seg]
    r1 = inner_rings[seg + 1]
    for i in range(SECTIONS):
        a, b = r0[(i+1)%SECTIONS], r0[i]
        c, d = r1[i], r1[(i+1)%SECTIONS]
        add_quad(a, b, c, d)

# ============================================================
# Bottom cap at z=0 (normal -Z): annulus between inner[0] and outer[0]
# ============================================================
o0 = outer_rings[0]
i0 = inner_rings[0]
for i in range(SECTIONS):
    a = o0[i]
    b = i0[i]
    c = i0[(i+1)%SECTIONS]
    d = o0[(i+1)%SECTIONS]
    add_quad(a, b, c, d)  # a→b→c→d gives -Z normal

# ============================================================
# Top cap at z=155 (normal +Z): annulus between inner[-1] and outer[-1]
# ============================================================
o_top = outer_rings[-1]
i_top = inner_rings[-1]
for i in range(SECTIONS):
    a = o_top[i]
    b = o_top[(i+1)%SECTIONS]
    c = i_top[(i+1)%SECTIONS]
    d = i_top[i]
    add_quad(a, b, c, d)  # a→b→c→d gives +Z normal

# ============================================================
# WRITE STL
# ============================================================
with open(OUT, 'w') as f:
    f.write("solid gun\n")
    for t in faces:
        a, b, c = [verts[i] for i in t]
        ax, ay, az = a
        bx, by, bz = b
        cx, cy, cz = c
        nx = (by - ay) * (cz - az) - (bz - az) * (cy - ay)
        ny = (bz - az) * (cx - ax) - (bx - ax) * (cz - az)
        nz = (bx - ax) * (cy - ay) - (by - ay) * (cx - ax)
        mag = math.sqrt(nx*nx + ny*ny + nz*nz)
        if mag > 0:
            nx /= mag; ny /= mag; nz /= mag
        f.write(f"  facet normal {nx:.6f} {ny:.6f} {nz:.6f}\n")
        f.write("    outer loop\n")
        f.write(f"      vertex {a[0]:.6f} {a[1]:.6f} {a[2]:.6f}\n")
        f.write(f"      vertex {b[0]:.6f} {b[1]:.6f} {b[2]:.6f}\n")
        f.write(f"      vertex {c[0]:.6f} {c[1]:.6f} {c[2]:.6f}\n")
        f.write("    endloop\n")
        f.write("  endfacet\n")
    f.write("endsolid gun\n")

print(f"Saved {OUT}")
print(f"Triangles: {len(faces)}")
print(f"Vertices:  {len(verts)}")
print(f"Z range: {Z_BARREL_START} to {Z_GRIP_END} mm")
print(f"Outer profile: {OUTER_PROFILE}")
print(f"Inner profile: {INNER_PROFILE}")