.. _`theory_measures`:

.. warning::

   Under Development

Measures
========

By combining the exposures and the associated causation factors, the number of ship-ship, drift and ram events can be calculated.
Measures can be applied to reduce the number of predicted accidents. Again, we distinguish between ship-ship and ship-object measures. 
Ship-object measures can be split into drift and ram measures. Some measures are enabled by default, others can be enabled optionally, 
but come with extra cost.

Nomenclature
------------

+----------------------------------+--------------------------------------+
| Symbol                           | Meaning                              |
+==================================+======================================+
| :math:`\mu`                      | Mean value                           |
+----------------------------------+--------------------------------------+
| :math:`\mu_d`                    | Drift velocity heading (going-to)    |
+----------------------------------+--------------------------------------+
| :math:`\mu_t`                    | Tide velocity heading (going-to)     |
+----------------------------------+--------------------------------------+
| :math:`\rho_{air}`               | Air density                          |
+----------------------------------+--------------------------------------+
| :math:`\rho_{water}`             | Sea water density                    |
+----------------------------------+--------------------------------------+
| :math:`\sigma`                   | Standard deviation value             |
+----------------------------------+--------------------------------------+
| :math:`\zeta`                    | Significant wave amplitude           |
+----------------------------------+--------------------------------------+
| :math:`f(x)`                     | Normal distribution                  |
+----------------------------------+--------------------------------------+
| :math:`g`                        | Gravitational acceleration           |
+----------------------------------+--------------------------------------+
| :math:`k`                        | Wave number                          |
+----------------------------------+--------------------------------------+
| :math:`s`                        | Drift distance                       |
+----------------------------------+--------------------------------------+
| :math:`t`                        | Time                                 |
+----------------------------------+--------------------------------------+
| :math:`v_{d}`                    | Drift speed                          |
+----------------------------------+--------------------------------------+
| :math:`\mathbf{v_d}`             | Drift velocity vector                |
+----------------------------------+--------------------------------------+
| :math:`v_{h}`                    | High tide velocity                   |
+----------------------------------+--------------------------------------+
| :math:`v_{l}`                    | Low tide velocity                    |
+----------------------------------+--------------------------------------+
| :math:`v_{t}`                    | Tidal speed                          |
+----------------------------------+--------------------------------------+
| :math:`\mathbf{v_d}`             | Tidal velocity vector                |
+----------------------------------+--------------------------------------+
| :math:`v_{w}`                    | Wind speed                           |
+----------------------------------+--------------------------------------+
| :math:`x`                        | Ram distance                         |
+----------------------------------+--------------------------------------+
| :math:`A_{L}`                    | Lateral area of the superstructure   |
+----------------------------------+--------------------------------------+
| :math:`B`                        | Breadth                              |
+----------------------------------+--------------------------------------+
| :math:`C_{w}`                    | Wind drag coefficient                |
+----------------------------------+--------------------------------------+
| :math:`C_{d}`                    | Hull drag coefficient                |
+----------------------------------+--------------------------------------+
| :math:`F_{w}`                    | Wind force                           |
+----------------------------------+--------------------------------------+
| :math:`F_{s}`                    | Wave drift force                     |
+----------------------------------+--------------------------------------+
| :math:`F_{d}`                    | Hull drag force                      |
+----------------------------------+--------------------------------------+
| :math:`H`                        | Height                               |
+----------------------------------+--------------------------------------+
| :math:`L`                        | Length                               |
+----------------------------------+--------------------------------------+
| :math:`P_{en}`                   | Probability engine repair            |
+----------------------------------+--------------------------------------+
| :math:`P_{an}`                   | Probability anchor failure           |
+----------------------------------+--------------------------------------+
| :math:`P_{av}`                   | Probability avoidance                |
+----------------------------------+--------------------------------------+
| :math:`P_{er}`                   | Probability ERTV deployment          |
+----------------------------------+--------------------------------------+
| :math:`P_{md}`                   | Probability all drift measures       |
+----------------------------------+--------------------------------------+
| :math:`P_{mr}`                   | Probability all ram measures         |
+----------------------------------+--------------------------------------+
| :math:`P_{ms}`                   | Probability all ship-ship measures   |
+----------------------------------+--------------------------------------+
| :math:`P_{pi}`                   | Probability pilot measure            |
+----------------------------------+--------------------------------------+
| :math:`R`                        | Drift coefficient                    |
+----------------------------------+--------------------------------------+
| :math:`T`                        | Draft                                |
+----------------------------------+--------------------------------------+
| :math:`T_{p}`                    | Wave peak period                     |
+----------------------------------+--------------------------------------+

Ship-Ship Measures
------------------

The pilot assistance is the only measure to reduce the number of predicted ship-ship collisions:

1.  Pilot assistance (optionally)

The ship-ship collision measure probability (:math:`P_{ms}`) only depends on the pilot assistance function: 

.. math::

    P_{ms} = P_{pi}

Pilot assistance
^^^^^^^^^^^^^^^^

The :term:`pilot` boards a ship and temporarily joins the crew to safely guide the passage of the ship. The pilot's knowledge and expertise 
reduces the probability of an accident between two ships.

.. math:: 
    :nowrap:
    
    \[
    P_{pi} = \begin{cases}
               \begin{split}
                 1.0 &\text{, disabled } \\
                 0.x &\text{, enabled } 
               \end{split}
             \end{cases} 
    \] 

Ship-Object Measures
--------------------

Drift
^^^^^

Measures can be taken to prevent a collision between the ship and a static object during a drift event. These measures reduce the probability 
of a collision. Some of measures are enabled by default, while other measures can be added optionally. The engine repair function and the 
anchor capability are default enabled. The presence of an ERTV and pilot assistance are optional, but imply cost to reduce the drift event probability.

The following four measures are or can be enabled to reduce the number of predicted ship object collisions due to drift events: 

1.  Engine repair measure (default enabled)
2.  Deploy an anchor (default enabled)
3.  Deploy an ERTV (optionally)

The drift measure probability (:math:`P_{md}`) can be computed by combining the probabilities of the above measures as follows: 

.. math::

    P_{md} = P_{en}P_{an}P_{er}
 
Engine repair measure
"""""""""""""""""""""

In 2002 and 2003 the Dutch Coastguard collected data about drifting ships in the Dutch part of the North Sea. This data is presented in 
:cite:`Tak_2004`. The derived engine repair measure is only applicable for a drifting ship. In total 57 ships drifted on an annual basis 
according to this analysis.

.. math:: 
    :nowrap:
    
    \[
    P_{en}(t) = \begin{cases}
                  \begin{split}
                                               1 &\text{, for } t<900s \\
                    \frac{3600}{1.5(t-900)+3600} &\text{, for } t\geq 900s
                  \end{split}
                \end{cases} 
    \]              
            
With :math:`t` in seconds. 

The engine repair measure requires the input variable :math:`t` to estimate the probability of a collision. Here, :math:`t` represents the 
time interval between exiting a link or cell and colliding with a static object, as described in the :ref:`theory_area`. The distance between 
the two is determined through the exposure calculation. Using the drift velocity, the duration of the drift event (:math:`t`) can be computed. 
This velocity depends on the ship’s characteristics and environmental factors such as wind, waves, and (tidal) currents. The methodology is 
further detailed in this section.  

Wave and Wind
'''''''''''''

Drift velocity concept is derived from :cite:`Remery_1973`. During a drift event no propulsion forces act on the ship. The velocity is a result 
of the equilibrium of the environmental forces acting on a ship body. These forces orginate from wind loads on the superstructure, wave drift 
forces and tidal current loads on the hull. It is assumed that wind and wave drift forces are aligned. The tidal current velocity heading is 
independent of the wind- and wave heading. The drift velocity vector of a ship is perpendicular to the ship's centreline.

.. math::
    :label: eq_sso_sum_forces

    F_{w} + F_{s} - F_{d} = 0

.. math:: 
    :label: eq_sso_wind_force

    F_{w}=\frac{1}{2}\rho_{air}A_{l}C_{w}v_{w}^2

With:

.. math:: 
    :label: eq_sso_wind_lat

    A_{l}=\left(H-T\right)L
    
.. math:: 
    :label: eq_sso_drift_force

    F_{s}=\frac{1}{16}\rho_{water}g{\zeta}^2LR^2  
    
.. math:: 
    :label: eq_sso_current_force

    F_{d}=\frac{1}{2}\rho_{water}TLC_{d}v_{d}^2

.. math::
    :label: eq_sso_drift_vel
    
    v_{d}=\sqrt{\frac{\rho_{air}}{\rho_{water}}\frac{A_{l}}{TL}\frac{C_{w}}{C_{d}}v_{w}^2+\frac{1}{8}\frac{{\zeta}^2g}{T}\frac{R^2}{C_{d}}} 

The wave drift coefficent (:math:`R`) is based on empirical coefficients :cite:`Remery_1973` and applied for all ship types and classes.

.. math:: 
    :label: eq_sso_drift_R
    
    R = \begin{cases}
          \begin{split}
            -1.4736(kT)^3+2.4765(kT)^2-0.0315(kT) &\text{, for } kT\leq1 \\
                                                 1 &\text{, for } kT>1 \\
          \end{split}
        \end{cases} 

For deep water the wave number (:math:`k`) is:

.. math::
    :label: eq_sso_wave_num

    k=\frac{(2\pi)^2}{T_{p}^2g}
    
.. table:: Wave peak period and wind speed for each Beaufort class

    +----------------+---------------+---------------+
    | Beaufort Class | :math:`T_{p}` | :math:`v_{w}` |
    +================+===============+===============+
    |   0            | NA            | NA            |
    +----------------+---------------+---------------+
    |   1            | 0.78          | 1.003         |
    +----------------+---------------+---------------+
    |   2            | 1.87          | 2.57          |
    +----------------+---------------+---------------+
    |   3            | 3.06          | 4.37          |
    +----------------+---------------+---------------+
    |   4            | 4.62          | 6.95          |
    +----------------+---------------+---------------+
    |   5            | 6.21          | 9.77          |
    +----------------+---------------+---------------+
    |   6            | 7.70          | 12.59         |
    +----------------+---------------+---------------+
    |   7            | 9.24          | 15.68         |
    +----------------+---------------+---------------+
    |   8            | 10.81         | 19.02         |
    +----------------+---------------+---------------+
    |   9            | 12.44         | 22.62         |
    +----------------+---------------+---------------+
    |  10            | 14.09         | 26.47         |
    +----------------+---------------+---------------+
    |  11            | 15.79         | 30.58         |
    +----------------+---------------+---------------+   

Tide
''''

The tide velocity cycle is simplified with a sine function. The low- and high tide values are set with the :math:`v_{l}` and the :math:`v_{h}` parameters.   

.. math:: 
    :label: eq_sso_tide_vel

    v_{t}=\frac{v_{l}+v_{h}}{2}+\frac{v_{h}-v_{l}}{2}sin\left(\frac{2{\pi}t}{t_n}\right), \text{ with } t_n=45000s 

Where :math:`t` is time in seconds.

Finally, the drift speed is a superposition of the computed velocity due to wind and waves (:math:`v_{d}`) and projected tidal velocity 
(:math:`v_{t}`). The tidal velocity vector is projected on the aligned wind- and wave vectors. The longitudinal component of this projection, 
which acts in the longitudinal direction of the ship, is ignored. 

The drift velocity vector is computed as follows:

.. math::
    :label: eq_sso_wave_vec
    
    \mathbf{v_d}= \begin{bmatrix}
                    v_d\,cos\left( {\mu}_d \right)\\
                    v_d\,sin\left( {\mu}_d \right)
                  \end{bmatrix}

The tidal velocity vector is computed as follows:
    
.. math::
    :label: eq_sso_tide_vec
    
    \mathbf{v_t}= \begin{bmatrix}   
                    v_t\,cos\left( {\mu}_t \right)\\
                    v_t\,sin\left( {\mu}_t \right)
                  \end{bmatrix}

A scalar (:math:`a`) is computed to scale the unit vector of the drift velocity: 

.. math::
    :label: eq_sso_tide_sca
    
    a=\frac{\mathbf{v}_t \cdot \mathbf{v}_d}{\left\| \mathbf{v}_d \right\|}

The projected tidal velocity vector is computed with the scalar from :math:numref:`eq_sso_tide_sca` and the normalized drift velocity vector 
:math:numref:`eq_sso_wave_vec`.
    
.. math::
    :label: eq_sso_tide_pro 

    \textit{proj}_\mathbf{v_d}\mathbf{v_t}=a\hat{\mathbf{v}}{\mathbf{_d}}  
    
.. math::
    :label: eq_sso_tide_pro_vec

    \textit{proj}_\mathbf{v_d}\mathbf{v_t}=v_t\left( v_d cos(\mu_d)cos(\mu_t) + v_d sin(\mu_d)sin(\mu_t) \right) \hat{\mathbf{v}}{\mathbf{_d}}    

The first term of :math:numref:`eq_sso_tide_pro_vec` can be substituted by :math:numref:`eq_sso_tide_vel`. The second term is a constant 
and is for simplicity replaced by variable :math:`c` in the next equation.

.. math::
    :label: eq_sso_tide_pro_fun
    
    \textit{proj}_\mathbf{v_d}{v_t} = c\,v_t

.. math::
    :label: eq_sso_tide_int

    s = \int_{t_0}^{t_1} \left( c\,v_t(t)+v_d \right) dt 

In total thirteen initial starting conditions of the drift event in a tidal cycle are considered:

.. math::
    :label: eq_sso_tide_cycle

    t_0 = \frac{n}{N}t_n, \text{ with } n \in \left\{0,1,2,..,12 \right\} \text{ and } N=13

The elapsed time (:math:`t_1-t_0`) is computed with the Newton-Raphson method for each starting condition of :math:`t_0` by solving 
:math:`t_1` numerically. 

Anchor deployment
"""""""""""""""""

An anchor can be deployed to prevent the ship from drifting in the event of engine failure. 

.. table:: Probability of an anchor failure for each Beaufort scale

    +----------------+---------------+
    | Beaufort Class | :math:`P_{an}`|
    +----------------+---------------+
    |   0            | 0.010         |
    +----------------+---------------+
    |   1            | 0.010         |
    +----------------+---------------+
    |   2            | 0.010         |
    +----------------+---------------+
    |   3            | 0.010         |
    +----------------+---------------+
    |   4            | 0.035         |
    +----------------+---------------+
    |   5            | 0.070         |
    +----------------+---------------+
    |   6            | 0.126         |
    +----------------+---------------+
    |   7            | 0.210         |
    +----------------+---------------+
    |   8            | 0.350         |
    +----------------+---------------+
    |   9            | 0.490         |
    +----------------+---------------+
    |  10            | 0.630         |
    +----------------+---------------+
    |  11            | 0.700         |
    +----------------+---------------+
    |  12            | 1.000         |
    +----------------+---------------+

Emergency Rescue Towing Vessel
""""""""""""""""""""""""""""""

.. note::

    Description will be updated in the near future by the newly developed model. 

The coastguard can deploy an Emergency Rescue Towing Vessel (ERTV) to assist a drifting ship. 

.. math:: 
    :nowrap:
    
    \[
    P_{er} = \begin{cases}
               \begin{split}
                 1.0 &\text{, disabled } \\
                 0.x &\text{, enabled } 
               \end{split}
             \end{cases} 
    \]              

Ram
^^^

The avoidance function is the only measure to reduce the number of predicted ship object collision due to ram events:

1.  Avoidance function (default enabled)

2.  Pilot assistance (optionally)

The ram measure probability (:math:`P_{mr}`) can be computed by combining the probabilities of the above measures as follows: 

.. math::

    P_{mr} = P_{av}P_{pi}

Avoidance function
""""""""""""""""""

The ram model has the avoidance function measure to reduce the probability of a collision due to a ram event. The probability is calculated as 
follows:
    
.. math:: P_{av}=e^{-a\frac{x}{L}} \text{ , } a=0.1

Variable :math:`L` corresponds to the length of the ship. The value of :math:`x` is the distance between the static object and the intersection 
of the course with the link from which the ram event started. 

Pilot assistance
""""""""""""""""

The pilot boards a ship and temporarily joins the crew to safely guide the passage of the ship. The pilot's knowledge and expertise 
reduces the probability of an accident with a static object.

.. math:: 
    :nowrap:
    
    \[
    P_{pi} = \begin{cases}
               \begin{split}
                 1.0 &\text{, disabled } \\
                 0.x &\text{, enabled } 
               \end{split}
             \end{cases} 
    \]   