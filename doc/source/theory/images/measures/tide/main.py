import numpy as np
import matplotlib.pyplot as plt
from scipy import optimize
import scipy.integrate as integrate

plt.close('all')

mu_t = np.radians(90)
mu_d = np.radians(110)
v_d = 0.25
v_l = 1.20
v_h = 1.50
t_0_arr = np.linspace(0, 45000, 13, endpoint=False)
t_n = 45000
v_d_vec = v_d * np.array([np.cos(mu_d), np.sin(mu_d)])
v_d_vec_norm = v_d_vec / np.sqrt(v_d_vec[0]**2+v_d_vec[1]**2)
s = 700

c = v_d * (np.cos(mu_d)*np.cos(mu_t) + np.sin(mu_d)*np.sin(mu_t))  # fraction


def f(x, c, v_l, v_h, t_n, t_0, s):
    t = (v_h+v_l)/2*c*x - c*(v_h-v_l)/2*t_n/(2*np.pi)*np.cos(2*np.pi*x/t_n) + v_d*x - \
        s - (v_h+v_l)/2*c*t_0 + c*(v_h-v_l)/2*t_n / \
        (2*np.pi)*np.cos(2*np.pi*t_0/t_n) - v_d*t_0
    return t


def g(x, c, v_l, v_h, v_d):
    return (v_h+v_l)*c/2 + (v_h-v_l)/2*c*np.sin(2*np.pi*x/t_n) + v_d


t_0 = t_0_arr[5]
try:
    t_1 = optimize.newton(f, t_0, args=(c, v_l, v_h, t_n, t_0, s))
    delta_t = t_1-t_0
    print(delta_t)
    s_check = integrate.quad(g, t_0, t_1, args=(c, v_l, v_h, v_d))
    print(s_check[0]/s)
except:
    print('No solution')

X = np.linspace(0, 50000, 200)
Y = g(X, 1, v_l, v_h, 0)
plt.plot(X, Y)
plt.plot([0, 50000], [v_l, v_l], '-r')
plt.plot([0, 50000], [v_h, v_h], '-r')
