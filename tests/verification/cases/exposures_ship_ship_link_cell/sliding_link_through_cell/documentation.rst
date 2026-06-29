Sliding Link through Cell
-------------------------

General
^^^^^^^

:Objective:
  Verify the computed exposure frequency between a vessel assigned to a cell and a vessel assigned to a link.

:Criteria:
  The computed exposure frequency is evaluated by comparing it against the reference values produced by the test script.

A single vessel type is assigned to a cell with a specified density, while the other vessel type is assigned to 
a link with a specified frequency. In the initial configuration, the link head is aligned with the left (west) 
side of the cell.

The link is then translated in incremental steps until its start point aligns with the right (east) side of 
the cell. The resulting exposure frequency is assessed as a function of the longitudinal position of the link’s 
start point.

- For the first four samples, the exposure must increase linearly from zero.
- For samples four through seven, the exposure must remain constant.
- For samples seven through ten, the exposure must decrease linearly back to zero.


.. figure:: figure.svg
   :alt: figure
   :align: center

   Test set-up last sample

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

.. _fig_Comparison_Exposures_Link_Sliding_through_cell:

.. figure:: figure1.svg
   :alt: Comparison:Exposures:Link:Sliding:through:cell
   :align: center
     
   Exposures versus link sliding through cell

.. literalinclude:: .check_output.txt