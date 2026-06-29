import numpy as np
import matplotlib.pyplot as plt

plt.close('all')

def crossing(Q1, Q2, u1, u2, b1, b2, l1, l2, theta): 
   
    u12 = np.sqrt(u1 ** 2 + u2 ** 2 - 2 * u1 * u2 * np.cos(theta))
    
    D1_1 = l1 * u2 * np.sin(theta) / u12
    D1_2 = b1 * np.sqrt(1-(np.sin(theta)*u2/u12)**2)
    D1 = D1_1 + D1_2

    D2_1 = l2 * u1 * np.sin(theta) / u12
    D2_2 = b2 * np.sqrt(1-(np.sin(theta)*u1/u12)**2)
    D2 = D2_1 + D2_2

    D12 = D1 + D2

    P12 = D12 / np.sin(theta)
    N = P12 * Q1 * Q2 * u12 / (u1 * u2)
    
    return N

Q1 = 0.001 # [s-1] Passing ships per second
Q2 = 0.002 # [s-1] Passing ships per second
u1 = 9 # [ms-1] ships speed
u2 = 6 # [ms-1] ships speed
l1 = 150.0 # [m] ships length
l2 = 200.0 # [m] ships length
b1 = 30.0 # [m] ships breadth
b2 = 40.0 # [m] ships breadth

theta0 = 0  # [rad]
theta1 = 0.25 # [rad]
theta2 = np.pi - theta1  # [rad]
theta3 = np.pi  # [rad] 

theta01 = theta1 - theta0
theta23 = theta3 - theta2

thetas = np.linspace(theta0, theta3, 10000)
thetas = thetas[1:-1]

transfer = []

for theta in thetas:

    if theta==theta0 or theta==theta3:
        transfer.append(0)
    elif theta>theta0 and theta<theta1:
        transfer.append(np.sin(theta/theta01*np.pi/2)**2) 
    elif theta>theta2 and theta<theta3:
        transfer.append(np.sin(np.pi/2 * (1 + (theta-theta2)/theta23))**2)     
    else:
        transfer.append(1)
    
exposures_no_map = crossing(Q1, Q2, u1, u2, b1, b2, l1, l2, thetas)
exposures = exposures_no_map * transfer 

fig, ax1 = plt.subplots()
ax1.plot(thetas, exposures, '-b', label='Exposures * Transfer Function')
ax1.plot(thetas, exposures_no_map, ':b', label='Exposures')
ax1.set_xlabel(r'$\theta$' + ' [rad]')
ax1.set_xlim([0, np.pi])
ax1.set_ylim([0, 0.0004])
ax1.set_yticks([]) 
ax1.set_ylabel('Exposures [-]')
ax1.legend(loc='upper left')
ax2 = ax1.twinx()
ax2.plot(thetas, transfer, '-', color='orange', label='Transfer Function')  
ax2.plot([theta1, theta1],[0,9],':k')
ax2.plot([theta2, theta2],[0,9],':k')
ax2.set_ylim([0, 1.4])
ax2.set_ylabel('Transfer [-]')
ax2.legend(loc='upper right')
ax1.set_title('Exposures')

     
