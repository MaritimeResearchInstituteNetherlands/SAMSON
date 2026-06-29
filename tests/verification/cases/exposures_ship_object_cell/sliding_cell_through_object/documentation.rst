Sliding Cell through Object
---------------------------

General
^^^^^^^

:Objective:
  Assess whether objects positioned near a cell are correctly identified as exposures.
:Criteria:
  The calculated drift exposures must align with the expected behavior.

A rectangular cell moves stepwise through an object. During this motion, the cell transitions from being 
completely outside the object boundaries, to intersecting them, and finally to being fully enclosed within 
the object. The wind direction and thus the drift direction is from north to south. The computed exposures 
throughout this process are checked against the expected results.

.. figure:: figure.svg
   :alt: figure
   :align: center
    
   Test set-up

Input
^^^^^

.. csv-table:: windstrength.csv
   :file: ./Area/windstrength.csv
   :widths: auto
   :header-rows: 1
   
.. csv-table:: winddirection.csv
   :file: ./Area/winddirection.csv
   :widths: auto
   :header-rows: 1   

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
   
.. csv-table:: objects.csv
   :file: ./Area/objects.csv
   :widths: auto
   :header-rows: 1 

Result
^^^^^^

.. _fig_Drift_Cell_Object:

.. figure:: figure1.svg
   :alt: Drift:Cell:Object
   :align: center
     
   Drift exposures versus left bottom vertex of cell

.. literalinclude:: .check_output.txt