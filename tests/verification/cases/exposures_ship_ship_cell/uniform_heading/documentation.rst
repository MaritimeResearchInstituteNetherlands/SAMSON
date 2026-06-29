Uniform Heading Distribution
----------------------------

General
^^^^^^^

:Objective:
  Verify the computed crossing exposure frequency for two ships located within a single cell.
:Criteria:
  The calculated exposure frequency is compared against the reference value produced by the test script.

The density values for both ships within the cell are used as input to the exposure models, assuming 
a uniform heading distribution for each vessel. The resulting exposure is then evaluated by comparing 
it with the corresponding results from the test script.

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

.. csv-table:: shipcelldata.csv
   :file: ./ModelData/shipcelldata.csv
   :widths: auto
   :header-rows: 1
   
.. csv-table:: shipcells.csv
   :file: ./Traffic/shipcells.csv
   :widths: auto
   :header-rows: 1   

Result
^^^^^^

.. literalinclude:: .check_output.txt
