#!/usr/bin/env python3
"""Verify STL manifoldness, normals, wall thickness, holes."""
import sys
import math
from collections import defaultdict

def load_stl(path):
    """Load ASCII STL, return (verts, faces)."""
    verts = []
    faces = []
    current_face_verts = []
    
    with open(path) as f:
        for line in f:
            parts = line.strip().split()
            if not parts:
                continue
            if parts[0] == 'vertex':
                v = tuple(float(p) for p in parts[1:])
                current_face_verts.append(v)
            elif parts[0] == 'endfacet':
                # Store the face (vertices are in order)
                faces.append(current_face_verts[:])
                current_face_verts = []
    
    # Deduplicate vertices
    unique_verts = {}
    vert_indices = []
    for face in faces:
        new_face = []
        for v in face:
            if v not in unique_verts:
                unique_verts[v] = len(unique_verts)
            new_face.append(unique_verts[v])
        vert_indices.append(new_face)
    
    verts = [None] * len(unique_verts)
    for v, idx in unique_verts.items():
        verts[idx] = v
    
    return verts, vert_indices

def normalize(v):
    mag = math.sqrt(sum(x*x for x in v))
    if mag == 0:
        return v
    return tuple(x/mag for x in v)

def cross(a, b):
    return (a[1]*b[2]-a[2]*b[1], a[2]*b[0]-a[0]*b[2], a[0]*b[1]-a[1]*b[0])

def sub(a, b):
    return tuple(x-y for x, y in zip(a, b))

def dot(a, b):
    return sum(x*y for x, y in zip(a, b))

def edge_key(e):
    return tuple(sorted(e))

def check_manifold(verts, faces):
    """Check if mesh is manifold (each edge shared by exactly 2 faces)."""
    edge_faces = defaultdict(list)
    for idx, face in enumerate(faces):
        for j in range(3):
            e = edge_key((face[j], face[(j+1)%3]))
            edge_faces[e].append(idx)
    
    non_manifold = []
    for e, f_list in edge_faces.items():
        if len(f_list) != 2:
            non_manifold.append((e, f_list))
    return non_manifold

def check_normals(verts, faces):
    """Check if face normals are consistent (point outward)."""
    # Compute centroid
    cx = sum(v[0] for v in verts) / len(verts)
    cy = sum(v[1] for v in verts) / len(verts)
    cz = sum(v[2] for v in verts) / len(verts)
    
    issues = []
    for idx, face in enumerate(faces):
        a, b, c = [verts[i] for i in face]
        n = normalize(cross(sub(b, a), sub(c, a)))
        # Vector from centroid to face center
        center = tuple((a[k]+b[k]+c[k])/3 for k in range(3))
        to_center = sub(center, (cx, cy, cz))
        if dot(n, to_center) < 0:
            issues.append((idx, n))
    return issues

def check_wall_thickness(verts, faces):
    """Check wall thickness at various z-slices."""
    # Group vertices by z-slices
    z_radii = defaultdict(list)
    for v in verts:
        z_r = round(v[2], 1)
        r = math.sqrt(v[0]**2 + v[1]**2)
        z_radii[z_r].append(r)
    
    thicknesses = []
    for z in sorted(z_radii.keys()):
        radii = z_radii[z]
        thicknesses.append((z, min(radii), max(radii), max(radii)-min(radii)))
    return thicknesses

def check_holes(verts, faces):
    """Check for boundary edges (edges with only one face)."""
    edge_faces = defaultdict(list)
    for idx, face in enumerate(faces):
        for j in range(3):
            e = edge_key((face[j], face[(j+1)%3]))
            edge_faces[e].append(idx)
    
    holes = []
    for e, f_list in edge_faces.items():
        if len(f_list) == 1:
            holes.append(e)
    return holes

def main():
    path = sys.argv[1] if len(sys.argv) > 1 else '/home/medicon-paki/work/2026Bunkasai/2026Bunkasai_gun.stl'
    verts, faces = load_stl(path)
    
    print(f"Loaded {path}")
    print(f"  Triangles: {len(faces)}")
    print(f"  Unique vertices: {len(verts)}")
    
    # Bounding box
    xs = [v[0] for v in verts]
    ys = [v[1] for v in verts]
    zs = [v[2] for v in verts]
    print(f"  Bounding box: x [{min(xs):.2f}, {max(xs):.2f}] mm")
    print(f"                  y [{min(ys):.2f}, {max(ys):.2f}] mm")
    print(f"                  z [{min(zs):.2f}, {max(zs):.2f}] mm")
    
    # Manifold check
    non_manifold = check_manifold(verts, faces)
    if non_manifold:
        print(f"\n  ⚠ NON-MANIFOLD edges: {len(non_manifold)}")
        for e, f_list in non_manifold[:5]:
            print(f"    Edge {e}: faces {f_list}")
    else:
        print("\n  ✓ All edges shared by exactly 2 faces (manifold)")
    
    # Normal consistency
    normal_issues = check_normals(verts, faces)
    if normal_issues:
        print(f"\n  ⚠ {len(normal_issues)} faces have inverted normals")
        for idx, n in normal_issues[:5]:
            print(f"    Face {idx}: normal {n}")
    else:
        print("\n  ✓ All normals consistent (point outward)")
    
    # Wall thickness
    thicknesses = check_wall_thickness(verts, faces)
    if thicknesses:
        print(f"\n  Wall thickness at z-slices (z, r_min, r_max, wall):")
        for t in thicknesses[:15]:
            print(f"    z={t[0]:.1f}mm: wall={t[3]:.2f}mm")
    
    # Holes check
    holes = check_holes(verts, faces)
    if holes:
        print(f"\n  ⚠ Boundary edges (potential holes): {len(holes)}")
    else:
        print("\n  ✓ No boundary edges (closed mesh)")

if __name__ == '__main__':
    main()