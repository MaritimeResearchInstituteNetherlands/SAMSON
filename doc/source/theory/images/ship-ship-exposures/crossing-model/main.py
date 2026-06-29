import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Polygon

plt.close('all')

speeds = np.array([7, 3])
breadths = np.array([50, 16])
lengths = np.array([300, 100])
waypoints = np.array([[600, 600], [-300, 200], [200, -300]])
links = np.array([[waypoints[0], waypoints[2]],
                  [waypoints[1], waypoints[2]]])

ship = np.array([[-0.5, -0.3], [-0.4, -0.5], [0.3, -0.5], [0.4, -0.3],
                [0.5, 0.0], [0.4, 0.3], [0.3, 0.5], [-0.4, 0.5], [-0.5, 0.3]])
contour = np.array([[-0.5, -0.5], [0.5, -0.5], [0.5, 0.5], [-0.5, 0.5]])

plot_scale = 30


def create_vector(link):
    vector = link[1]-link[0]
    vector_len = np.sqrt(vector[0]**2 + vector[1]**2)
    vector_norm = vector / vector_len
    return vector_norm


def rotation_matrix_2D(psi, x0):
    R = np.array([[np.cos(psi), -np.sin(psi)],
                  [np.sin(psi), np.cos(psi)]])
    x1 = R.dot(x0)
    return x1


origin = []
dx = []
dy = []
angle = []
vector = []
u = []
for i in range(2):
    origin.append(links[i][0])
    dx.append(links[i, 1, 0] - links[i, 0, 0])
    dy.append(links[i, 1, 1] - links[i, 0, 1])
    angle.append(np.arctan2(dy[-1], dx[-1]))
    vector.append(create_vector(links[i, :, :]))
    u.append(vector[-1] * speeds[i])

u12 = u[1] - u[0]
speed12 = np.sqrt(u12[0]**2 + u12[1]**2)
angle12 = np.arctan2(u12[1], u12[0])
theta = np.arccos(np.dot(vector[0], vector[1]))

radius = []
for i in range(2):
    speed = np.roll(speeds, 1)[i]
    term1 = lengths[i] * speed * np.sin(theta) / speed12
    term2 = breadths[i] * np.sqrt(1 - (np.sin(theta) * speed / speed12)**2)
    radius.append((term1 + term2) / 2.0)

fig, ax = plt.subplots()

for i in range(2):
    if i == 0:
        label = 'Link'
    else:
        label = ''
    cont_coll = np.array(
        [contour[:, 0] * lengths[i], 2.0 * contour[:, 1] * radius[i]])
    cont_coll = rotation_matrix_2D(angle12, cont_coll)
    cont_coll[0, :] += origin[i][0]
    cont_coll[1, :] += origin[i][1]
    plot_coll = Polygon(np.transpose(cont_coll), facecolor='red', alpha=0.1)
    cont_poly = np.array(
        [contour[:, 0] * lengths[i], contour[:, 1] * breadths[i]])
    cont_poly = rotation_matrix_2D(angle[i], cont_poly)
    cont_poly[0, :] += origin[i][0]
    cont_poly[1, :] += origin[i][1]
    plot_cont = Polygon(np.transpose(cont_poly), facecolor='k')
    ship_poly = np.array([ship[:, 0] * lengths[i], ship[:, 1] * breadths[i]])
    ship_poly = rotation_matrix_2D(angle[i], ship_poly)
    ship_poly[0, :] += origin[i][0]
    ship_poly[1, :] += origin[i][1]
    plot_ship = Polygon(np.transpose(ship_poly), facecolor='g')
    plot_circ = plt.Circle(
        (origin[i][0], origin[i][1]), radius[i], color='red', alpha=0.2)
    ax.arrow(origin[i][0], origin[i][1], dx[i], dy[i],
             head_width=30, head_length=50, length_includes_head=True,
             fc='k', ec='k', label=label)
    ax.add_patch(plot_coll)
    ax.add_patch(plot_circ)
    ax.add_patch(plot_cont)
    ax.add_patch(plot_ship)
    ax.arrow(origin[i][0], origin[i][1], plot_scale*u[i][0],
             plot_scale*u[i][1], head_width=30, head_length=50,
             linewidth=3, fc='blue', ec='blue')

ax.arrow(origin[1][0], origin[1][1], plot_scale*u12[0], plot_scale*u12[1],
         linewidth=3, head_width=30, head_length=50, fc='blue', ec='blue',
         label='Velocity')
ax.text(-525, 225, r'$ship_i$', fontsize=16)
ax.text(-425, 125, r'$D_j$', fontsize=16)
ax.text(725, 675, r'$ship_j$', fontsize=16)
ax.text(725, 575, r'$D_i$', fontsize=16)
plt.show()
ax.axes.get_xaxis().set_visible(False)
ax.axes.get_yaxis().set_visible(False)
plt.title('Crossing Exposure Model')
plt.axis('equal')
plt.legend()
plt.grid()

fig, (ax1, ax2) = plt.subplots(1,2)

for i in range(2):
    if i == 0:
        label = 'Diameter'
    else:
        label = None
    ax1.arrow(origin[i][0], origin[i][1], dx[i], dy[i],
             head_width=30, head_length=50, length_includes_head=True,
             fc='k', ec='k')
    ax1.plot([origin[i][0] - radius[i] * np.cos(angle[i]),
              origin[i][0] + radius[i] * np.cos(angle[i])], 
             [origin[i][1] - radius[i] * np.sin(angle[i]),
              origin[i][1] + radius[i] * np.sin(angle[i])], 
              linewidth=5, color='r', label=label)
    ax1.arrow(origin[i][0], origin[i][1], plot_scale*u[i][0],
             plot_scale*u[i][1], head_width=30, head_length=50,
             linewidth=3, fc='blue', ec='blue')

ax1.arrow(origin[1][0], origin[1][1], plot_scale*u12[0], plot_scale*u12[1],
         linewidth=3, head_width=30, head_length=50, fc='blue', ec='blue',
         label='Velocity')    

ax1.legend()
ax1.set_xlim([-500, 750])
ax1.set_ylim([-700, 1100])
ax1.set_aspect('equal', adjustable='box')
ax1.axes.get_xaxis().set_visible(False)
ax1.axes.get_yaxis().set_visible(False)

link_length1 = np.sqrt((links[0,1,0]-links[0,0,0])**2 + (links[0,1,1]-links[0,0,1])**2)
link_length2 = np.sqrt((links[1,1,0]-links[1,0,0])**2 + (links[1,1,1]-links[1,0,1])**2)
u1 = np.sqrt(u[0][0]**2 + u[0][1]**2)
u2 = np.sqrt(u[1][0]**2 + u[1][1]**2)

ax2.arrow(0, link_length1, 0, -link_length1,
          head_width=30, head_length=50, length_includes_head=True,
          fc='k', ec='k', label=label)
ax2.arrow(0, 0, 0, -plot_scale*u1, head_width=30, head_length=50, linewidth=3, fc='blue', ec='blue')  
ax2.plot([0,0], [-radius[0], radius[0]], linewidth=5, color='r')

ax2.arrow(-link_length2, 0, link_length2, 0,
          head_width=30, head_length=50, length_includes_head=True,
          fc='k', ec='k', label=label)
ax2.arrow(-radius[1], 0, plot_scale*u2, 0, head_width=30, head_length=50, linewidth=3, fc='blue', ec='blue')
ax2.plot([-2*radius[1], 0], [0,0], linewidth=5, color='r')

ax2.set_xlim([-1000, 250])
ax2.set_ylim([-500, 1300])
ax2.set_aspect('equal', adjustable='box')
ax2.axes.get_xaxis().set_visible(False)
ax2.axes.get_yaxis().set_visible(False)