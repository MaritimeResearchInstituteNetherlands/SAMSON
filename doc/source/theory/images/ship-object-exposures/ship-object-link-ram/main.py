import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon as Patch
from shapely import Point
from shapely import LineString
from shapely import Polygon
from shapely import affinity

plt.close('all')

aabb_radius = 20000
rotate = 0.15*np.pi
sigma3 = 0.76
link_length = 8000
sublinks_y = np.linspace(-800, -4000, 5)
sublinks_dy = sublinks_y[1] - sublinks_y[0]

def gen_circle (x0, y0, radius):
    coords = []
    thetas = np.linspace(0, 2*np.pi, 64, endpoint=False)
    for itheta in thetas:
        x = x0 + radius * np.cos(itheta)
        y = y0 + radius * np.sin(itheta)
        coords.append([x, y])
    return Polygon(coords) 

def gen_aabb (x0, y0, x1, y1, radius):
    xmin = min(x0,x1)
    xmax = max(x0,x1)
    ymin = min(y0,y1)
    ymax = max(y0,y1)
    coords = [[xmin-radius, ymin-radius],
              [xmin-radius, ymax+radius],
              [xmax+radius, ymax+radius],
              [xmax+radius, ymin-radius]]
    return Polygon(coords)  

def gen_polygon (x0, y0, length, radius, sigma3):
    coords = [[0,0],
              [radius * np.cos(sigma3), radius * np.sin(sigma3)]]
    thetas = np.linspace(sigma3, 0, 12)
    x_mir = []
    y_mir = []
    for itheta in thetas:
        x = length + radius * np.cos(itheta)
        y = radius * np.sin(itheta)
        coords.append([x, y])
        x_mir.append(x)
        y_mir.append(y)
    x_mir = reversed(x_mir)
    y_mir = reversed(y_mir)   
    for ix, iy in zip(x_mir, y_mir):
        coords.append([ix, -iy])
    coords.append([radius * np.cos(-sigma3), radius * np.sin(-sigma3)])
    return Polygon(coords)

link = LineString([[0, 0], [link_length, 0]])
object_static = Polygon([[13000,6000], [17000,5000], [17500,2000], [14250, 1750]])
link_rotated = affinity.rotate(link, rotate, origin=[0,0], use_radians=True)

x0 = link_rotated.coords.xy[0][0]
y0 = link_rotated.coords.xy[1][0]
x1 = link_rotated.coords.xy[0][1]
y1 = link_rotated.coords.xy[1][1]
dx = x1 - x0
dy = y1 - y0

object_rotated = affinity.rotate(object_static, rotate, origin=[0,0], use_radians=True)  
aabb = gen_aabb(x0, y0, x1, y1, aabb_radius)  
circle0 = gen_circle (link_rotated.coords.xy[0][0], link_rotated.coords.xy[1][0], aabb_radius)
circle1 = gen_circle (link_rotated.coords.xy[0][1], link_rotated.coords.xy[1][1], aabb_radius)
polygon = gen_polygon(x0, y0, abs(dx), aabb_radius, sigma3)
polygon_rotated = affinity.rotate(polygon, rotate, origin=[0,0], use_radians=True)

fig, ax1 = plt.subplots()

ax1.arrow(x0, y0, dx, dy, linewidth=3, head_width=300, head_length=500, length_includes_head=True, fc='k', ec='k', label='Link')
ax1.plot(*object_rotated.exterior.xy, '-b',label='Object')
ax1.plot(*circle0.exterior.xy, ':k', label='Radius')
ax1.plot(*circle1.exterior.xy, ':k')
ax1.plot(*aabb.exterior.xy, '-r', label='AABB')
ax1.set_xlim([-3e4,3e4])
ax1.set_ylim([-3e4,3e4])
ax1.legend()
ax1.set_aspect('equal', adjustable='box')
ax1.axes.get_xaxis().set_visible(False)
ax1.axes.get_yaxis().set_visible(False)

fig, ax2 = plt.subplots()

ax2.arrow(x0, y0, dx, dy, linewidth=3, head_width=300, head_length=500, length_includes_head=True, fc='k', ec='k', label='Link')
ax2.plot(*object_rotated.exterior.xy, '-b',label='Object')
ax2.plot(*polygon_rotated.exterior.xy, '-r', label='Polygon')
ax2.set_xlim([-3e4,3e4])
ax2.set_ylim([-3e4,3e4])
ax2.legend()
ax2.set_aspect('equal', adjustable='box')
ax2.axes.get_xaxis().set_visible(False)
ax2.axes.get_yaxis().set_visible(False)

fig, ax3 = plt.subplots()
ax3.arrow(x0, y0, dx, dy, linewidth=3, head_width=300, head_length=500, length_includes_head=True, fc='k', ec='k', label='Link')
ax3.plot(*object_rotated.exterior.xy, '-b',label='Object')
thetas = []
for X0, Y0 in zip([x0,x1],[y0,y1]):
    for x, y in zip(*object_rotated.exterior.xy):
        ax3.plot([X0,x],[Y0,y],':k')
        thetas.append(np.arctan2(y-Y0, x-X0))
theta_min = np.array(thetas).argmin()
theta_max = np.array(thetas).argmax()
for i in (theta_min, theta_max):
    if i<len(object_rotated.exterior.xy[0]):
        index = i
        x = object_rotated.exterior.xy[0][index]
        y = object_rotated.exterior.xy[1][index]
        ax3.plot([x0,x],[y0,y],'-r', label='Boundary')
    else:
        index = i%len(object_rotated.exterior.xy[0])
        x = object_rotated.exterior.xy[0][index]
        y = object_rotated.exterior.xy[1][index]
        ax3.plot([x1,x],[y1,y],'-r', label='Boundary')             
ax3.set_xlim([-1e4,2e4])
ax3.set_ylim([-1e4,2e4])
ax3.legend()
ax3.set_aspect('equal', adjustable='box')
ax3.axes.get_xaxis().set_visible(False)
ax3.axes.get_yaxis().set_visible(False)

fig, ax4 = plt.subplots()
ax4.arrow(x0, y0, dx, dy, linewidth=3, head_width=300, head_length=500, length_includes_head=True, fc='k', ec='k', label='Link')
ax4.plot(*object_rotated.exterior.xy, '-b',label='Object')
for iy in sublinks_y:
    arrow = Point([0,iy-sublinks_dy/2])
    arrow_rotated = affinity.rotate(arrow, rotate, origin=[0,0], use_radians=True)
    sublink = Polygon([[0,iy-sublinks_dy/2], [link_length,iy-sublinks_dy/2], [link_length,iy+sublinks_dy/2], [0, iy+sublinks_dy/2]])
    sublink_rotated = affinity.rotate(sublink, rotate, origin=[0,0], use_radians=True)
    ax4.plot(*sublink_rotated.exterior.xy, ':g')
    x0 = np.sum(sublink_rotated.exterior.xy[0][3:6])/2
    y0 = np.sum(sublink_rotated.exterior.xy[1][3:6])/2
    dx = sublink_rotated.exterior.xy[0][1] - sublink_rotated.exterior.xy[0][0]
    dy = sublink_rotated.exterior.xy[1][1] - sublink_rotated.exterior.xy[1][0] 
    ax4.arrow(x0, y0, dx, dy, linewidth=1, head_width=300, head_length=500, length_includes_head=True, fc='g', ec='g', label='Sub-link')
ax4.legend()
ax4.set_aspect('equal', adjustable='box')
ax4.axes.get_xaxis().set_visible(False)
ax4.axes.get_yaxis().set_visible(False)  
ax4.text(7600,3600,r'$+3\sigma_{y}$')  
ax4.text(9200,0,r'$-3\sigma_{y}$')  

theta_low = thetas[theta_min]
theta_high = thetas[theta_max]
theta_range = theta_high-theta_low
theta_n = 4
theta_delta = theta_range / theta_n
thetas_rays = np.linspace(theta_low+theta_delta/2, theta_high-theta_delta/2, 4)
fig, ax5 = plt.subplots(ncols=theta_n)
for i, irays in enumerate(thetas_rays):
    object_rays = affinity.rotate(object_static, -irays+rotate, origin=[0,0], use_radians=True) 
    ax5[i].plot(*object_rays.exterior.xy, 'b')
    rays_x_max = max(object_rays.exterior.xy[0])
    rays_y_min = min(object_rays.exterior.xy[1])
    rays_y_max = max(object_rays.exterior.xy[1])
    link_rays = affinity.rotate(link, -irays+rotate, origin=[0,0], use_radians=True)
    plot_rays = Patch([[0,rays_y_min], [rays_x_max,rays_y_min], [rays_x_max,rays_y_max], [0, rays_y_max]], facecolor='k', alpha=0.1)
    ax5[i].add_patch(plot_rays)
    ax5[i].plot(*link_rays.coords.xy, 'k')
    ax5[i].set_xlim([-0.5e4,2.0e4])
    ax5[i].set_ylim([-1.5e4,1.0e4])
    ax5[i].set_aspect('equal', adjustable='box')
    ax5[i].axes.get_xaxis().set_visible(False)
    ax5[i].axes.get_yaxis().set_visible(False) 
    ax5[i].set_title('Bin, k=' + str(i))     
