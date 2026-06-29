Head-on Model
-------------

General
^^^^^^^

:Objective:
  Verify the computed head-on exposure frequency for two ships with the head-on model.
:Criteria:
  The calculated exposure frequency is compared with the computed value in the test script.  

Two links with shared waypoints in opposite directions are included in the area definition. Each link has a ship characterized by its velocity, 
width, and frequency. A normal lateral distribution is assumed, with both ship types having defined mean values (:math:`\mu`) and standard deviations 
(:math:`\sigma`). The calculated exposure frequency is compared with the value computed in the test script and must fall within the specified tolerances.

.. figure:: figure.svg
   :alt: figure
   :align: center
    
   Test set-up
    
Input
^^^^^

.. csv-table:: shipcategories.csv
   :file: ./Traffic/shipcategories.csv
   :widths: auto
   :header-rows: 1

.. csv-table:: shiplinkdata.csv
   :file: ./ModelData/shiplinkdata.csv
   :widths: auto
   :header-rows: 1
   
.. csv-table:: shiplinks.csv
   :file: ./Traffic/shiplinks.csv
   :widths: auto
   :header-rows: 1  

Result
^^^^^^

.. literalinclude:: .check_output.txt