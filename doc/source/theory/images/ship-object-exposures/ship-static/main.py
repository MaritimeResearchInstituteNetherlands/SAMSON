import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon

plt.close('all')
ram_angle = np.radians(30)


mp = np.dtype(str, metadata={'object_type': 'monopile'})
an = np.dtype(str, metadata={'object_type': 'anchor'})
pl = np.dtype(str, metadata={'object_type': 'platform'})
de = np.dtype(str, metadata={'object_type': 'depth'})
# x = np.array([1, 2, 4], dtype=dt)
# print(x.dtype.metadata)


def rotation_matrix_2D(psi, x0):
    R = np.array([[np.cos(psi), -np.sin(psi)],
                  [np.sin(psi), np.cos(psi)]])
    x1 = R.dot(x0)
    return x1


def line_line_intersection(line0, line1):
    # https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection
    x1 = line0[0][0]
    x2 = line0[1][0]
    x3 = line1[0][0]
    x4 = line1[1][0]
    y1 = line0[0][1]
    y2 = line0[1][1]
    y3 = line1[0][1]
    y4 = line1[1][1]
    px_nom = (x1*y2 - y1*x2) * (x3 - x4) - (x1 - x2) * (x3*y4 - y3*x4)
    py_nom = (x1*y2 - y1*x2) * (y3 - y4) - (y1 - y2) * (x3*y4 - y3*x4)
    denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4)
    if denom != 0:
        x = px_nom / denom
        y = py_nom / denom
        p = np.array([x, y])
    else:
        p = np.array([])
    return p


fig, ax = plt.subplots(1, 1)

N = 12
radius_monopile = 5.00
radius_ship = 50
r = radius_monopile + radius_ship
xy_monopile_offset = [200, 300]

angle_start = 2*np.pi/N/2
angle_end = 2 * np.pi - angle_start
angle = np.linspace(angle_start, angle_end, N)
xy_monopile = radius_monopile * np.array([np.cos(angle), np.sin(angle)])
xy_monopile[0, :] += xy_monopile_offset[0]
xy_monopile[1, :] += xy_monopile_offset[1]
xy_monopile = xy_monopile.T
xy_monopile_contour = r * np.array([np.cos(angle), np.sin(angle)])
xy_monopile_contour[0, :] += xy_monopile_offset[0]
xy_monopile_contour[1, :] += xy_monopile_offset[1]
xy_monopile_contour = xy_monopile_contour.T
xy_monopile_edges = []
for i in range(len(xy_monopile_contour)):
    xy_monopile_edges.append(np.roll(xy_monopile_contour, -i, axis=0)[0:2, :])
xy_monopile_edges = np.array(xy_monopile_edges)

polygon0 = Polygon(xy_monopile, facecolor='black', label='Monopile')
ax.add_patch(polygon0)
polygon1 = Polygon(xy_monopile_contour, facecolor='none',
                   edgecolor='black', linewidth=2, linestyle='-', alpha=1.0)
ax.add_patch(polygon1)

length = 40
width = 30
xy_platform_offset = np.array([500, 300])
xy_platform_angle = np.deg2rad(-50)
xy_platform = np.array([[-length/2.0, -width/2.0],
                        [length/2.0, -width/2.0],
                        [length/2.0,  width/2.0],
                        [-length/2.0,  width/2.0]])
xy_platform_contour = np.array([[-length/2.0-r, -width/2.0-r],
                                [length/2.0+r, -width/2.0-r],
                                [length/2.0+r, width/2.0+r],
                                [-length/2.0-r, width/2.0+r]])
xy_platform = rotation_matrix_2D(xy_platform_angle, xy_platform.T)
xy_platform[0, :] += xy_platform_offset[0]
xy_platform[1, :] += xy_platform_offset[1]
xy_platform_contour = rotation_matrix_2D(
    xy_platform_angle, xy_platform_contour.T)
xy_platform_contour[0, :] += xy_platform_offset[0]
xy_platform_contour[1, :] += xy_platform_offset[1]
xy_platform_contour = xy_platform_contour.T
xy_platform_edges = []
for i in range(len(xy_platform_contour)):
    xy_platform_edges.append(np.roll(xy_platform_contour, -i, axis=0)[0:2, :])
xy_platform_edges = np.array(xy_platform_edges)

polygon2 = Polygon(xy_platform.T, facecolor='blue', label='Platform')
ax.add_patch(polygon2)
polygon3 = Polygon(xy_platform_contour, facecolor='none',
                   edgecolor='blue', linewidth=2, linestyle='-', alpha=1.0)
ax.add_patch(polygon3)

xy_anchor = np.array([[1100, 200],
                      [1250, 175],
                      [1300, 400],
                      [1120, 380],
                      [1050, 325],
                      [1000, 325]])
edges = []
for i in range(len(xy_anchor)):
    two_pnts = np.roll(xy_anchor, -i, axis=0)[0:2, :]
    dx = two_pnts[1][0] - two_pnts[0][0]
    dy = two_pnts[1][1] - two_pnts[0][1]
    le = np.sqrt(dx**2 + dy**2)
    vector = np.array([dy, -dx])/le
    edges.append(two_pnts + radius_ship * vector)
xy_anchor_contour = []
for i in range(len(xy_anchor)):
    two_pnts = np.roll(edges, -i, axis=0)[0:2, :]
    intersection = line_line_intersection(two_pnts[0], two_pnts[1])
    xy_anchor_contour.append(list(intersection))
xy_anchor_contour = np.array(xy_anchor_contour)
xy_anchor_edges = []
for i in range(len(xy_anchor_contour)):
    xy_anchor_edges.append(np.roll(xy_anchor_contour, -i, axis=0)[0:2, :])
xy_anchor_edges = np.array(xy_anchor_edges)

polygon4 = Polygon(xy_anchor, facecolor='green', label='Anchor')
ax.add_patch(polygon4)
polygon5 = Polygon(xy_anchor_contour, facecolor='none',
                   edgecolor='green', linewidth=2, linestyle='-', alpha=1.0)
ax.add_patch(polygon5)

xy_depth = np.array([[-200, 650],
                     [200, 875],
                     [650, 625],
                     [1800, 675]])
xy_depth_contour = np.copy(xy_depth)
xy_depth_contour[:, 1] += -radius_ship
ax.plot(xy_depth[:, 0], xy_depth[:, 1], 'orange', linestyle=':', label='Depth')
ax.plot(xy_depth_contour[:, 0], xy_depth_contour[:, 1],
        'orange', linestyle='-', linewidth=2)

fig, ax1 = plt.subplots(1, 1)

colors = ['black', 'blue', 'green', 'orange']
labels = ['Monopile', 'Platform', 'Anchor', 'Depth']

items = [np.vstack((xy_monopile_contour, xy_monopile_contour[0, :])),
         np.vstack((xy_platform_contour, xy_platform_contour[0, :])),
         np.vstack((xy_anchor_contour, xy_anchor_contour[0, :])),
         xy_depth_contour]

for i, item in enumerate(items):
    x_values = []
    dis_values = []
    for ivertex, vertex in enumerate(item):
        a = np.tan(ram_angle)
        b = vertex[1] - a*vertex[0]
        x = -b / a
        x0 = vertex[0]
        y0 = vertex[1]
        x1 = x
        y1 = 0
        dx = x1 - x0
        dy = y1 - y0
        dis = np.sqrt(dx**2 + dy**2)
        x_values.append(x)
        dis_values.append(dis)
        ax.plot([vertex[0], x], [vertex[1], 0], 'grey', linewidth=0.5)
    ax1.plot(x_values, dis_values, colors[i], linewidth=1.0, label=labels[i])

ax.arrow(0, 0, 1500, 0, linewidth=5, head_width=30, head_length=60,
         length_includes_head=True, fc='k', ec='k')
ax1.arrow(0, 0, 1500, 0, linewidth=5, head_width=30, head_length=60,
          length_includes_head=True, fc='k', ec='k')

ax.axis('equal')
ax.legend()

ax1.axis('equal')
ax1.legend()

# for i, item in enumerate(items):
#     items_mod = np.copy(items)
#     items_mod = np.delete(items_mod, i)
#     for j in range(len(items_mod)-1):

        # edge0 = items_mod[j:j+2]
        # for line0 in item:
        #     for line1 in item_mod:
        #         print(line0, line1)
        #         p = line_line_intersection(line0, line1)
        #         cond = np.zeros(8, dtype=bool)
        #         if len(p) != 0:
        #             cond[0] = p[0] >= min(line0[:, 0])
        #             cond[1] = p[0] <= max(line0[:, 0])
        #             cond[2] = p[1] >= min(line0[:, 1])
        #             cond[3] = p[1] <= max(line0[:, 1])
        #             cond[4] = p[0] >= min(line1[:, 0])
        #             cond[5] = p[0] <= max(line1[:, 0])
        #             cond[6] = p[1] >= min(line1[:, 1])
        #             cond[7] = p[1] <= max(line1[:, 1])
        #             print(cond)
        #         inter_bool = bool(np.prod(cond))

        # xy_platform_edges_bool = np.ones(len(xy_monopile_contour), dtype=bool)
        # for i in range(len(xy_monopile_contour)):
        #     vertex = xy_monopile_contour[i]
        #     a = np.tan(ram_angle)
        #     b = vertex[1] - a*vertex[0]
        #     x = -b / a
        #     line0 = np.array([[vertex[0], vertex[1]], [x, 0]])
        #     edges = np.roll(xy_monopile_edges, -i, axis=0)[1:-1, :]
        #     for edge in edges:
        #         line1 = edge
        #         p = line_line_intersection(line0, line1)
        #         cond = np.zeros(4, dtype=bool)
        #         if len(p) != 0:
        #             cond[0] = p[0] >= min(edge[:, 0])
        #             cond[1] = p[0] <= max(edge[:, 0])
        #             cond[2] = p[1] >= min(edge[:, 1])
        #             cond[3] = p[1] <= max(edge[:, 1])
        #             print(cond)
        #         inter_bool = bool(np.prod(cond))
        #         if inter_bool:
        #             xy_platform_edges_bool[i] = False
        #             break
