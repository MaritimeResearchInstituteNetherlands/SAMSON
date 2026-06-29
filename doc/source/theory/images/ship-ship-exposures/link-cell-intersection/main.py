import numpy as np
import shapely
import shapely.plotting
from shapely.geometry import LineString, Polygon
import matplotlib.pyplot as plt
from scipy.stats import norm

# Close all existing plots
plt.close('all')
fig, ax1 = plt.subplots()

# Define mean (mu) and standard deviation (sigma) for the normal distribution
link = np.array([[-400, 0], [400, -200]])
mu = -200
sigma = 60
b0 = 33.4
b1 = 20.0

# Define radius and angle for the cell
radius = 350
cell_link_angle_offset = np.radians(30)
nd_bins_num = 15

b01 = (b0 + b1) / 2

# Calculate the cumulative distribution function (CDF) values for the normal distribution
nd_low = norm.cdf(mu-3*sigma, loc=mu, scale=sigma)
nd_high = 1.0 - nd_low
nd_tot = 1.0 - 2.0*nd_low
nd_prob_delta = 1.0 / nd_bins_num
nd_bins_size = nd_tot/nd_bins_num
nd_bins_edges = np.linspace(nd_low, nd_high, nd_bins_num + 1)

# Calculate the edges and widths of the bins in the normal distribution
nd_bins_edge_cdf = []
nd_bins_cdf_width = []
for i in nd_bins_edges:
    nd_bins_edge_cdf.append(norm.ppf(i, loc=mu, scale=sigma))
    if len(nd_bins_edge_cdf) > 1:
        nd_bins_cdf_width.append(nd_bins_edge_cdf[-1] - nd_bins_edge_cdf[-2])

# Define the main link and calculate its properties
link_shape = LineString(link)
link_length = link_shape.length
link_dx = link[1, 0] - link[0, 0]
link_dy = link[1, 1] - link[0, 1]
link_angle = np.arctan2(link_dy, link_dx)

tag_text = [r'$-3\sigma$', r'$\mu$', r'$3\sigma$']
tag_loc = [mu-3*sigma, mu, mu+3*sigma]
for i, j in zip(tag_loc, tag_text):
    offset = -100
    nd_bounds_shape = LineString([[0, i], [offset, i]])
    nd_bounds_shape = shapely.affinity.rotate(
        nd_bounds_shape, link_angle, origin=(0, 0), use_radians=True)
    nd_bounds_shape = shapely.affinity.translate(
        nd_bounds_shape, xoff=link[0, 0], yoff=link[0, 1])
    shapely.plotting.plot_line(
        nd_bounds_shape, add_points=False, linestyle=':', color='k')
    ax1.text(nd_bounds_shape.xy[0][1],
             nd_bounds_shape.xy[1][1], j, horizontalalignment='right', fontsize=10)

# Define the coordinates for the distribution area and rotate it by the link angle
link_dist_xy = np.array([[0, mu+3*sigma],
                         [link_length, mu+3*sigma],
                         [link_length, mu-3*sigma],
                         [0, mu-3*sigma]])
link_dist_xy[:, 0] = link_dist_xy[:, 0] + link[0, 0]
link_dist_xy[:, 1] = link_dist_xy[:, 1] + link[0, 1]
link_dist_shape = Polygon(link_dist_xy)
link_dist_shape = shapely.affinity.rotate(
    link_dist_shape, link_angle, origin=(link[0, 0], link[0, 1]), use_radians=True)

# Define the coordinates for a hexagon and calculate its area
cell_hex_nor_xy = np.array([
    [1.0, 0.0],
    [0.5, np.sqrt(3)/2],
    [-0.5, np.sqrt(3)/2],
    [-1, 0],
    [-0.5, -np.sqrt(3)/2],
    [0.5, -np.sqrt(3)/2]])
cell_hex_xy = radius * cell_hex_nor_xy
cell_hex_shape = Polygon(cell_hex_xy)
cell_hex_shape_area = cell_hex_shape.area

# Plot the bins and their intersections with the cell distribution area
for i in range(nd_bins_num):
    y = np.roll(np.array(nd_bins_edge_cdf), -i)[0:2]
    box = np.array([[0, y[1]],
                   [link_length, y[1]],
                   [link_length, y[0]],
                   [0, y[0]]])
    box[:, 0] = box[:, 0] + link[0, 0]
    box[:, 1] = box[:, 1] + link[0, 1]
    box_shape = Polygon(box)
    box_shape = shapely.affinity.rotate(
        box_shape,
        link_angle,
        origin=(link[0, 0], link[0, 1]),
        use_radians=True)
    box_prob = shapely.intersection(box_shape, cell_hex_shape)
    length = box_prob.area / nd_bins_cdf_width[i]
    if i == 0:
        label = 'Intersecting polygon'
    else:
        label = ''
    alpha_set = 0.25 + 0.25 * (i % 2)
    shapely.plotting.plot_polygon(
        box_prob,
        ax=ax1,
        add_points=False,
        facecolor='blue',
        edgecolor='blue',
        alpha=alpha_set,
        label=label)
    if i == 0:
        label = 'Equal probability bin, K=' + '{:.0f}'.format(nd_bins_num)
    else:
        label = ''
    ax1.plot(*box_shape.exterior.xy, '-g', label=label)

ax1.arrow(link[0, 0],
          link[0, 1],
          link_dx,
          link_dy,
          head_width=25,
          head_length=50,
          length_includes_head=True,
          fc='k',
          ec='k',
          label='Link')
ax1.plot(*cell_hex_shape.exterior.xy, '-b', linewidth=2, label='Cell')
ax1.axis('equal')
ax1.legend()
ax1.axes.get_xaxis().set_visible(False)
ax1.axes.get_yaxis().set_visible(False)

fig, ax2 = plt.subplots()

box_prob = shapely.affinity.rotate(
    box_prob, -link_angle, origin=(0, 0), use_radians=True)
box_prob = shapely.affinity.translate(
    box_prob,
    xoff=-box_prob.centroid.xy[0][0],
    yoff=-box_prob.centroid.xy[1][0])

shapely.plotting.plot_polygon(
    box_prob,
    ax=ax2,
    add_points=False,
    facecolor='blue',
    edgecolor='blue',
    alpha=alpha_set,
    label='Intersecting polygon')


bin_width = max(box_prob.exterior.xy[1]) - min(box_prob.exterior.xy[1])
cell_link_length = bin_width / np.sin(cell_link_angle_offset)
cell_link_shape0 = LineString([[-cell_link_length/2, 0],
                              [cell_link_length/2, 0]])
cell_link_shape0 = shapely.affinity.rotate(
    cell_link_shape0, cell_link_angle_offset, origin=(0, 0), use_radians=True)

cell_link_shape1 = LineString([[min(box_prob.exterior.xy[0]), 0],
                              [max(box_prob.exterior.xy[0]), 0]])
cell_link_shape1 = shapely.intersection(cell_link_shape1, box_prob)

cell_link_shapes = [cell_link_shape0, cell_link_shape1]
cell_link_labels = ['Link of bin', 'Link']
cell_link_colors = ['blue', 'k']
cell_link_data = zip(cell_link_shapes, cell_link_labels, cell_link_colors)
for cell_link_shape, cell_link_label, cell_link_color in cell_link_data:
    cell_link_x0 = cell_link_shape.xy[0][0]
    cell_link_y0 = cell_link_shape.xy[1][0]
    cell_link_x1 = cell_link_shape.xy[0][1]
    cell_link_y1 = cell_link_shape.xy[1][1]
    cell_link_dx = cell_link_x1 - cell_link_x0
    cell_link_dy = cell_link_y1 - cell_link_y0
    ax2.arrow(cell_link_x0,
              cell_link_y0,
              cell_link_dx,
              cell_link_dy,
              head_width=25,
              head_length=50,
              length_includes_head=True,
              fc=cell_link_color,
              ec=cell_link_color,
              label=cell_link_label)
ax2.text(80, 5, r'$\theta$', horizontalalignment='center', fontsize=16)
ax2.axis('equal')
ax2.legend()
ax2.axes.get_xaxis().set_visible(False)
ax2.axes.get_yaxis().set_visible(False)
