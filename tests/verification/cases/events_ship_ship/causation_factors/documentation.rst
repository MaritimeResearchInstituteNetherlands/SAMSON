Striking and Struck Probability
-------------------------------

General
^^^^^^^

:Objective:
  Verify the computed event rate for two ships derived from crossing model.
:Criteria:
  The calculated event rate is compared with the computed value in the test script. 

Two links are connected through a shared waypoint. The relative angle between them is varied from 0deg to 360deg in 10deg increments. 
The outputs from all 37 configurations are verified against the event rate calculated in the test script. The exposure frequency from the 
output are collected and multiplied with the specified causation factors to compute the event rate. This rate is compared with the 
values in the output.

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

.. csv-table:: causationfactors.csv
   :file: ./Causation/causationfactors.csv
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
