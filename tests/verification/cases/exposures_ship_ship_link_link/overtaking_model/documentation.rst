Overtaking Model
----------------

General
^^^^^^^

:Objective:
  Verify the computed overtaking exposure frequency for two ships with the overtaking model.
:Criteria:
  The calculated exposure frequency is compared with the computed value in the test script.  

The area definition includes a single link on which two distinct ship types operate. Each ship is characterized by its velocity, 
width, and frequency. A normal lateral distribution is assumed, with both ship types having specified mean values and standard 
deviations. The computed exposure frequency is then compared to the value produced by the test script and must fall within the defined 
tolerance range.

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