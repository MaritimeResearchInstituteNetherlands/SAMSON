import time

import matplotlib.pyplot as plt
import numpy as np
from scipy.stats import multivariate_normal
from shapely.geometry import Point, Polygon


plt.close("all")

start_time = time.time()

# ----------------------------------------------------------------------
# Constants
# ----------------------------------------------------------------------
SIGMA_X = 3.0
SIGMA_Y = 3.0
MU_X = 5.0
MU_Y = 5.0
RADIUS = 10.0
THETA = 4 / 8 * np.pi

GRID_M = 12
GRID_N = 12
SIGMA_N = 3

CENTER = Point(0.0, 0.0)
CIRCLE = CENTER.buffer(RADIUS, quad_segs=64)
XC, YC = CIRCLE.exterior.xy

# ----------------------------------------------------------------------
# Grid for contour plot
# ----------------------------------------------------------------------
x_shear = np.tan(np.pi / 2 - THETA)
x_scale = 1 / np.sin(THETA)

x_arr = np.linspace(
    -SIGMA_N * SIGMA_X,
    SIGMA_N * SIGMA_X,
    num=GRID_M + 1,
)

y_arr = np.linspace(
    -SIGMA_N * SIGMA_Y,
    SIGMA_N * SIGMA_Y,
    num=GRID_N + 1,
)

x_grid_orth, y_grid_orth = np.meshgrid(x_arr, y_arr)

mu = [0.0, 0.0]
sigma = [
    [SIGMA_X ** 2, 0.0],
    [0.0, SIGMA_Y ** 2],
]

rv = multivariate_normal(mu, sigma)

# ----------------------------------------------------------------------
# Transform grid
# ----------------------------------------------------------------------
x_grid = x_scale * x_grid_orth
x_grid += x_shear * y_grid_orth
x_grid += x_scale * MU_X

y_grid = y_grid_orth + MU_Y

r = np.sqrt(x_grid ** 2 + y_grid ** 2)
inside_circle = r < RADIUS

xp0, yp0 = x_grid_orth[0, 0], y_grid_orth[0, 0]
xp1, yp1 = x_grid_orth[0, -1], y_grid_orth[0, -1]
xp2, yp2 = x_grid_orth[-1, -1], y_grid_orth[-1, -1]
xp3, yp3 = x_grid_orth[-1, 0], y_grid_orth[-1, 0]

XP0, YP0 = x_grid[0, 0], y_grid[0, 0]
XP1, YP1 = x_grid[0, -1], y_grid[0, -1]
XP2, YP2 = x_grid[-1, -1], y_grid[-1, -1]
XP3, YP3 = x_grid[-1, 0], y_grid[-1, 0]

para_pts = [(XP0, YP0), (XP1, YP1), (XP2, YP2), (XP3, YP3)]
parallelogram = Polygon(para_pts)

has_intersection = parallelogram.intersection(CIRCLE).area != 0.0
grid_outside_circle = has_intersection and np.sum(inside_circle) == 0

cell_area = (XP1 - XP0) * (YP2 - YP1)

area_prob = (
    + rv.cdf([xp0, yp0])
    - rv.cdf([xp1, yp1])
    + rv.cdf([xp2, yp2])
    - rv.cdf([xp3, yp3])
)

area_cell = cell_area / (GRID_M * GRID_N)
correction = 1.0 / area_prob

# ----------------------------------------------------------------------
# Plotting setup
# ----------------------------------------------------------------------
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 8))

cell_ratios = np.zeros((GRID_N, GRID_M))
probability_sum = 0.0

# ----------------------------------------------------------------------
# Loop through grid cells
# ----------------------------------------------------------------------
if has_intersection:
    for i in range(GRID_N):
        for j in range(GRID_M):

            cell_ratio = 0.0

            vertices = np.array(
                [
                    inside_circle[i, j],
                    inside_circle[i + 1, j],
                    inside_circle[i + 1, j + 1],
                    inside_circle[i, j + 1],
                ]
            )
            vertices_inside = np.sum(vertices)

            if vertices_inside != 0 or grid_outside_circle:

                x0, y0 = x_grid_orth[i, j], y_grid_orth[i, j]
                x1, y1 = x_grid_orth[i, j + 1], y_grid_orth[i, j + 1]
                x2, y2 = x_grid_orth[i + 1, j + 1], y_grid_orth[i + 1, j + 1]
                x3, y3 = x_grid_orth[i + 1, j], y_grid_orth[i + 1, j]

                X0, Y0 = x_grid[i, j], y_grid[i, j]
                X1, Y1 = x_grid[i, j + 1], y_grid[i, j + 1]
                X2, Y2 = x_grid[i + 1, j + 1], y_grid[i + 1, j + 1]
                X3, Y3 = x_grid[i + 1, j], y_grid[i + 1, j]

                prob = (
                    + rv.cdf([x0, y0])
                    - rv.cdf([x1, y1])
                    + rv.cdf([x2, y2])
                    - rv.cdf([x3, y3])
                )

                grid_polygon = Polygon([(X0, Y0), (X1, Y1), (X2, Y2), (X3, Y3)])

                if vertices_inside == 4:
                    cell_ratio = 1.0
                else:
                    intersection = grid_polygon.intersection(CIRCLE)
                    cell_ratio = intersection.area / area_cell

                probability_sum += prob * cell_ratio

            cell_ratios[i, j] = cell_ratio

total_probability = correction * probability_sum
elapsed_time = time.time() - start_time

# ----------------------------------------------------------------------
# Output
# ----------------------------------------------------------------------
percentage_inside = np.sum(inside_circle) / inside_circle.size
print(f"Percentage of enclosed grid points: {percentage_inside:.1%}")
print(f"Probability: {total_probability:.3f}")
print(f"Elapsed time: {elapsed_time:.3f} s")

# ----------------------------------------------------------------------
# Plotting
# ----------------------------------------------------------------------
fig.suptitle(f"Probability: {total_probability:.3f}", fontsize=12)

im1 = ax1.pcolormesh(
    x_grid,
    y_grid,
    cell_ratios,
    vmin=0.0,
    vmax=1.0,
    shading="flat",
    cmap="jet",
)

fig.colorbar(im1, ax=ax1, label="Ratio")
ax1.plot(XC, YC, ":k", label="Waypoint Circle")
ax1.plot(x_grid, y_grid, "ok", markersize=0.5)
ax1.set_title("Grid Cell Intersection Ratio")
ax1.set_aspect("equal")
ax1.grid(True)
ax1.legend()

pos = np.dstack((x_grid_orth, y_grid_orth))
im2 = ax2.contourf(
    x_grid_orth,
    y_grid_orth,
    rv.pdf(pos),
    cmap="jet",
)

fig.colorbar(im2, ax=ax2, label="PDF")
ax2.plot(x_grid_orth, y_grid_orth, "ok", markersize=0.5)
ax2.set_xlabel("x")
ax2.set_ylabel("y")
ax2.set_title("2D Multivariate Normal Distribution")
ax2.set_aspect("equal")
ax2.grid(True)

plt.show()
