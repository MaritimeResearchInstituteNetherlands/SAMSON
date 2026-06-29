Sliding Link over Two Cells
---------------------------

General
^^^^^^^

:Objective:
  Verify the computed exposure frequency between two cells and a link.

:Criteria:
  The computed exposure frequency is evaluated by comparing it against the reference values produced by the test script.

Two adjacent cells contain identical density specifications for a given ship type. The ship type and frequency 
are assigned to a link, which is shifted longitudinally in incremental steps. The starting position is defined 
as the moment the link is fully enclosed by the left cell, and the ending position is when it is fully enclosed 
by the right cell. For each experiment, the total exposure should remain identical. 


.. figure:: figure.svg
   :alt: figure
   :align: center

   Test set-up last experiment

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

.. _fig_Comparison_Exposures_Link_Sliding_through_cells:

.. figure:: figure1.svg
   :alt: Comparison:Exposures:Link:Sliding:through:cells
   :align: center
     
   Exposures versus link’s first longitude coordinate

.. literalinclude:: .check_output.txt