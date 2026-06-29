.. _`theory_causation`:

.. warning::

   Under Development

Causation Factors
=================

The causation factors are derived from the ratio between the observed exposures and the true accidents in an area. Two exposure types are specified, 
denoted as ship-ship and ship-static object. Therefore causation factors are established for both two types. A distinction can be made according to ship 
size and type. 

The multiplication of the ship-ship exposures (collison candidates) with the causation factor results in a number of predicted events in the area of 
interest. Ship-static object exposures (danger miles) with the causation factor provides you the number of ships that drift/ram a static 
object, if no action is undertaken. Measures can be taken to reduce the number of accidents. These measures are discussed in the 
:ref:`theory_measures` section.

Nomenclature
------------

+------------------------+-------------------------------------------------------------------------+
| Symbol                 | Meaning                                                                 |
+========================+=========================================================================+
| :math:`C_a^b(s,t)`     | Ship-ship events per time unit for :math:`a,b,s,t`                      |
+------------------------+-------------------------------------------------------------------------+
| :math:`C_o`            | Ship-object events per time unit                                        |
+------------------------+-------------------------------------------------------------------------+
| :math:`C_x`            | Ship-ship events per time unit for :math:`x`                            |
+------------------------+-------------------------------------------------------------------------+
| :math:`F_a^b(s,t)`     | Ship-ship causation factor for :math:`a,b,s,t`                          |
+------------------------+-------------------------------------------------------------------------+
| :math:`F_o`            | Ship-object causation factor                                            |
+------------------------+-------------------------------------------------------------------------+
| :math:`F_x`            | Ship-ship causation factor for :math:`x`                                |
+------------------------+-------------------------------------------------------------------------+
| :math:`L`              | Length of a projected static object line segment on a link              |
+------------------------+-------------------------------------------------------------------------+
| :math:`N_a^b(s,t)`     | Ship-ship exposures per time unit for :math:`a,b,s,t`                   |
+------------------------+-------------------------------------------------------------------------+
| :math:`N_x`            | Ship-ship exposures per time unit for :math:`x`                         |
+------------------------+-------------------------------------------------------------------------+
| :math:`Q`              | Number of passages per time unit for each ship type and size on a link  |
+------------------------+-------------------------------------------------------------------------+


Ship-Ship Causation Factors
---------------------------

Ships are assigned to links and cells. This is traffic in an area (see section :ref:`theory_traffic`). The passages per unit time (:math:`Q_{ij}`) 
and the lateral distibution parameters (:math:`\mu_{ij}` and :math:`\sigma_{ij}`) are specified per link (:math:`i`) and ship type (:math:`j`). The cell 
density (:math:`D_{ij}`) represents the travelled distance per unit area and unit time.
The traffic output properties are used to determine the exposures, denoted as collision candidates for the ship-ship exposure type. The output of a ship-ship 
exposure model is a rate (number of exposures per time unit). 

.. math::   
    :label: eq_CF_1
    
    N_a^b(s,t) \text{ with } a \in {RR,NN,RN,NR} \text{ and } b \in {OV,CR,HO} 

And ship types :math:`s,t` where ship type :math:`s` collides with ship type :math:`t`.

The clarification of all elements in :math:`a` is listed below. 

.. table:: Elements of :math:`a`

    +-------------+-----------------+-----------------+
    | :math:`a`   | ship :math:`s`  | ship :math:`t`  |
    +=============+=================+=================+
    | :math:`RR`  | Route bound     | Route bound     |
    +-------------+-----------------+-----------------+
    | :math:`NN`  | Non-route bound | Non-route bound |
    +-------------+-----------------+-----------------+
    | :math:`RN`  | Route bound     | Non-route bound |
    +-------------+-----------------+-----------------+
    | :math:`NR`  | Non-route bound | Route bound     |
    +-------------+-----------------+-----------------+

The clarification of all elements in :math:`b` is listed below. 

.. table:: Elements of :math:`b`

    +------------+------------+
    | :math:`b`  | type       |
    +============+============+
    | :math:`OV` | Overtaking |
    +------------+------------+
    | :math:`CR` | Crossing   |
    +------------+------------+
    | :math:`HO` | Head-on    |
    +------------+------------+

Special Case
~~~~~~~~~~~~

Suppose that for each combination of :math:`a,b,s,t` the number of actual collisions in the area per time unit is known as 
:math:`C_a^b(s,t)`. Then we can determine the causation factor using:

.. math::   
    :label: eq_CF_2
    
    F_a^b(s,t) = \frac{C_a^b(s,t)}{N_a^b(s,t)}

A dimension analysis of :math:`F_a^b(s,t)` results in:
    
.. math::

    \frac{ \frac{\text{number of true collisions}}{\text{time unit}} }{ \frac{\text{number of collision candidates}}{\text{time unit}} }

Which can be simplified to: 

.. math::

    \frac{\text{number of true collisions}}{\text{number of collision candidates}}      

General Case
~~~~~~~~~~~~

In practice it is often the case that :math:`C_a^b(s,t)` is not known for each single combination of :math:`a,b,s,t`. Let :math:`G` be the set of all 
possible combinations of :math:`a,b,s,t`.

Suppose that we have a disjunct partition of :math:`G` in :math:`X` parts:

.. math::   
    :label: eq_CF_3
    
     G = \bigcup_{x=1}^X G_x 

And corresponding collision statistics :math:`C_x` for each :math:`G_x`, assuming that the causation factor for all members of :math:`G_x` is equal. We 
can calculate the corresponding collision candidates.

.. math::   
    :label: eq_CF_4

    N_x = \sum_{(a,b,s,t) \in G_x} N_a^b(s,t)

And the corresponding causation factors:

.. math::   
    :label: eq_CF_5

    F_x = \frac{C_x}{N_x}

Below the number of ship-ship collisions at the North Sea on an annual bases are listed. In this case the number of composed groups is 9 (:math:`X=9`).  

.. table:: Average number of collisions per year in the period 2005-2016

    +-------------------------------------------+--------------------------------------+------------+
    | Angle                                     | Collisions                           | Total      |
    +                                           +------------+------------+------------+            |
    |                                           | RR         | RN/NR      | NN         |            |
    +===========================================+============+============+============+============+
    | **OV** :math:`0deg\leq\theta<60 deg`      | 0.65       | 0.20       | 0.21       | 1.06       |
    +-------------------------------------------+------------+------------+------------+------------+
    | **CR** :math:`60deg\leq\theta<150 deg`    | 0.83       | 1.39       | 0.64       | 2.86       |
    +-------------------------------------------+------------+------------+------------+------------+
    | **HO** :math:`150deg\leq\theta\leq180 deg`| 0.19       | 0.00       | 0.64       | 0.83       |
    +-------------------------------------------+------------+------------+------------+------------+
    | All                                       | 1.67       | 1.58       | 1.50       | 4.75       |
    +-------------------------------------------+------------+------------+------------+------------+

Once the causation factors in an area have been identified, the ship-ship event rates can be calculated using the exposure rates.

.. math:: 
    :label: eq_CF_6
    
    C_x = F_xN_x

.. admonition:: Example

    This example serves to illustrate the underlying concept using a minimal configuration. The objective is conceptual clarity 
    rather than quantitative accuracy. The modeled area comprises three pairs of two links.
    
        #.  Link :math:`A` and :math:`B`, connected via one shared waypoint node (:math:`\theta=30deg`).
        
        #.  Link :math:`C` and :math:`D`, connected via one shared waypoint node (:math:`\theta=100deg`).
        
        #.  Link :math:`E` and :math:`F`, connected via two shared waypoint nodes (:math:`\theta=180deg`).
    
    Three ship types (:math:`i`, :math:`j` and :math:`k`) operate in the area. The following traffic is composed with 
    ship types assigned to links: 
        
        *  Link :math:`A`: ship :math:`i`
        
        *  Link :math:`B`: ship :math:`j` and :math:`k`
        
        *  Link :math:`C`: ship :math:`i`

        *  Link :math:`D`: ship :math:`k`
        
        *  Link :math:`E`: ship :math:`i`
        
        *  Link :math:`F`: ship :math:`j`
    
    This yields the following set of exposures and some arbitrarily assigned exposure frequency values and fractions. For clarity, 
    we substitute the :math:`RR` notation with the link tags. 
    
        *  Link pair :math:`AB`, computed with the crossing ships model:
            
            *  :math:`N_{AB}^{OV}(i,j) = 10`
            
                *  The bow of ship :math:`i` collides with ship :math:`j`: :math:`\frac{D_{i}}{D_{ij}} = 0.6`
                
                *  The bow of ship :math:`j` collides with ship :math:`i`: :math:`\frac{D_{j}}{D_{ij}} = 0.4`
            
            *  :math:`N_{AB}^{OV}(i,k) = 2`
            
                *  The bow of ship :math:`i` collides with ship :math:`k`: :math:`\frac{D_{i}}{D_{ik}} = 0.2`
                
                *  The bow of ship :math:`k` collides with ship :math:`i`: :math:`\frac{D_{k}}{D_{ik}} = 0.8`
            
        *  Link :math:`B`, computed with the overtaking ships model:
        
            *  :math:`N_{B}^{OV}(j,k) = 2`
            
                *  The bow of ship :math:`j` collides with the stern of ship :math:`k`: :math:`f_j = 0.0`
                
                *  The bow of ship :math:`k` collides with the stern of ship :math:`j`: :math:`f_k = 1.0`
            
        *  Link pair :math:`CD`, computed with the crossing ships model:
            
            *  :math:`N_{CD}^{CR}(i,k) = 6`
            
                *  The bow of ship :math:`i` collides with ship :math:`k`: :math:`\frac{D_{i}}{D_{ik}} = 0.7`
                
                *  The bow of ship :math:`k` collides with ship :math:`i`: :math:`\frac{D_{k}}{D_{ik}} = 0.3`
        
        *  Link pair :math:`EF`, computed with the head-on ships model:
        
            *  :math:`N_{EF}^{HO}(i,j) = 4`
            
                *  The bow of ship :math:`i` collides with the bow of ship :math:`j`: :math:`f_i = 0.5`
                
                *  The bow of ship :math:`j` collides with the bow of ship :math:`i`: :math:`f_j = 0.5`
   
    The following observations can be made from this review:
    
        *  The crossing exposure model can be applied to derive overtaking- and head-on exposure frequencies, depending on the value of :math:`\theta`.

        *  The speed of ship :math:`k` on link :math:`B` is larger than the speed of ship :math:`j`. Ship :math:`k` is for this pair always the striking ship.
        
        *  The computed exposure frequency by the head-on model is equally distributed over both ships. 
    
    We list our exposure frequencies of struck ships as follows:
    
        *  :math:`N_{A}^{OV}(i) = 0.4 \cdot 10 = 4`
        
        *  :math:`N_{B}^{OV}(j) = 0.6 \cdot 10 = 6`
        
        *  :math:`N_{A}^{OV}(i) = 0.8 \cdot 2 = 1.6`
        
        *  :math:`N_{B}^{OV}(k) = 0.2 \cdot 2 = 0.4`
        
        *  :math:`N_{B}^{OV}(j) = 1.0 \cdot 2 = 2`
        
        *  :math:`N_{B}^{OV}(k) = 0.0 \cdot 2 = 0`
        
        *  :math:`N_{C}^{CR}(i) = 0.3 \cdot 6 = 1.8`
        
        *  :math:`N_{D}^{CR}(k) = 0.7 \cdot 6 = 4.2`
        
        *  :math:`N_{E}^{HO}(i) = 0.5 \cdot 4 = 2`
        
        *  :math:`N_{F}^{HO}(j) = 0.5 \cdot 4 = 2`
    
    The following set of arbitrarily assigned :math:`RR` accident frequency values has been recorded in the area:
   
        *  Two (2) accidents between ship :math:`i` and ship :math:`j` per time unit.
        
            *  The bow of ship :math:`i` collides with the stern of ship :math:`j` (:math:`\theta=30deg`).

            *  The bow of ship :math:`i` collides with the bow of ship :math:`j` (:math:`\theta=165deg`).
        
        *  One (1) accident between ship :math:`i` and ship :math:`k` per time unit.
            
            *  The bow of ship :math:`k` collides with the side of ship :math:`i` (:math:`\theta=90deg`).

    To derive the collision frequencies, the total number of recorded accidents was distributed with the following rules:
    
        *  Bow-bow contact: evenly distributed between the two ship types.
        
        *  Bow-stern contact: fully assigned to the struck ship with damage at the stern.
        
        *  Bow-side contact: fully assigned to the struck ship with damage at the side.
    
    Yielding the following results for our example:

        *  :math:`C_{RR}^{OV}(j) = 1`
        
        *  :math:`C_{RR}^{HO}(i) = 0.5`
        
        *  :math:`C_{RR}^{HO}(j) = 0.5`
        
        *  :math:`C_{RR}^{CR}(i) = 1`

    If we group ship types :math:`i`, :math:`j` and :math:`k`, we can compute the causation factors as follows:
    
        *  :math:`F_{RR}^{OV}(i,j,k) = \frac{C_{RR}^{OV}(i,j,k)}{N_{RR}^{OV}(i,j,k)} = \frac{1}{4+6+1.6+0.4+2+0} = \frac{1}{14}`
        
        *  :math:`F_{RR}^{CR}(i,j,k) = \frac{C_{RR}^{CR}(i,j,k)}{N_{RR}^{CR}(i,j,k)} = \frac{1}{1.8+4.2} = \frac{1}{6}`
                
        *  :math:`F_{RR}^{HO}(i,j,k) = \frac{C_{RR}^{HO}(i,j,k)}{N_{RR}^{HO}(i,j,k)} = \frac{1}{2+2} = \frac{1}{4}`
        
    Herewith the collision frequencies of struck ships can be computed again. The sum of the computed collisions of all involved ships is :math:`3`. 
    
        *  :math:`C_{A}^{OV}(i) = F_{RR}^{OV}(i,j,k) \cdot N_{A}^{OV}(i) = \frac{4}{14}`
        
        *  :math:`C_{B}^{OV}(j) = F_{RR}^{OV}(i,j,k) \cdot N_{B}^{OV}(j) = \frac{6}{14}`
        
        *  :math:`C_{A}^{OV}(i) = F_{RR}^{OV}(i,j,k) \cdot N_{A}^{OV}(j) = \frac{1.6}{14}`
        
        *  :math:`C_{B}^{OV}(k) = F_{RR}^{OV}(i,j,k) \cdot N_{B}^{OV}(k) = \frac{0.4}{14}`

        *  :math:`C_{B}^{OV}(j) = F_{RR}^{OV}(i,j,k) \cdot N_{B}^{OV}(j) = \frac{2}{14}`
        
        *  :math:`C_{B}^{OV}(k) = F_{RR}^{OV}(i,j,k) \cdot N_{B}^{OV}(k) = \frac{0}{14}`
        
        *  :math:`C_{C}^{CR}(i) = F_{RR}^{CR}(i,j,k) \cdot N_{C}^{CR}(i) = \frac{1.8}{6}`
        
        *  :math:`C_{D}^{CR}(k) = F_{RR}^{CR}(i,j,k) \cdot N_{D}^{CR}(k) = \frac{4.2}{6}`
        
        *  :math:`C_{E}^{HO}(i) = F_{RR}^{HO}(i,j,k) \cdot N_{E}^{HO}(i) = \frac{2}{4}`
        
        *  :math:`C_{F}^{HO}(j) = F_{RR}^{HO}(i,j,k) \cdot N_{F}^{HO}(j) = \frac{2}{4}`

Ship-Object Causation Factors
-----------------------------

The ship-object causation factor is defined as a probability per unit length. The unit length represents a travelled distance by a ship. The 
probability indicates the occurence of a drift and/or ram event. It can be approached using two different ways:

1. Define the engine/steering failure probability per nautical mile from literature.

2. Same as for ship-ship, compute the ratio between the travelled distance and the observed drift- and ram events.

This can be formulated as follows for the first method:

.. math::
    :label: eq_CF_8

    F_{o} = \frac{P_e}{\text{unit length}} 

Where :math:`{P_e}` is the probability that an event occurs. This causation factor (:math:`F_{o}`) can also be computed with the ratio of observed 
drift/ram events and the total travelled distances by all ships in an area for a particular time span (second method). 

.. math::
    :label: eq_CF_9

    F_{o} = \frac {\frac{\text{total number of observed events}}{\text{time unit}}} {\frac{\text{total travelled distance}}{\text{time unit}}}

Which can be simplified to: 

.. math::

    \frac {\text{total number of observed events}} {\text{total travelled distance}}  

Once the causation factor in an area has been identified, the ship-object event rates can be calculated using the exposure rates. The ship-object 
event rate is defined as follows:

.. math:: 
    :label: eq_CF_10
    
    C_{o} = F_{o}LQ

Where :math:`L` is length of the projected segment from a static object on a link and :math:`Q` is the number of passages per time unit for each 
ship type and size for this link. A dimension analysis of :math:numref:`eq_CF_10` results in:

.. math::

    \frac{\text{events}}{\text{length unit}}\cdot{\text{length unit}}\cdot{\frac{\text{passing ships}}{\text{time unit}}}
    
Which can be simplified to:

.. math::
    
    \frac{\text{events}}{\text{time unit}}
    








