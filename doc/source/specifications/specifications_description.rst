.. _`specification_description`:

Description
=========== 

.. warning::

   Under Development

User Classes and Characteristics
--------------------------------

Area
~~~~ 

The Area component serves as the geographical context within which ship movements, static objects, and weather phenomena are monitored 
and analyzed to predict collision frequencies. 

*  The World Geodetic System 1984 (WGS 84) for global positioning, ensuring accurate representation of locations shall be used. 

*  A raster grid system shall be implemented to divide the area into manageable sectors for detailed analysis: 

   *  Start: is the latitude – longitude coordinate of the start point of the raster 
    
   *  End: is the latitude – longitude coordinate of the endo point of the raster 
 
   *  Origin: is indication of the direction of the raster. By default it starts from South-West. 

   *  Grid Number in Latitude Direction (Y-axis): The total number of grid cells spanning the latitude of the area covered by the raster. 

   *  Grid Number in Longitude Direction (X-axis): The total number of grid cells spanning the longitude of the area covered by the raster.  

*  Each grid cell shall be defined using geographic coordinates (latitude and longitude) and have configurable dimensions by user. 

Traffic Network
~~~~~~~~~~~~~~~ 

A traffic network consists 3 types of items; waypoints, links and raster. 

A way point shall have following attributes: 

*  Latitude: Floating-point number representing the latitude of the waypoint. 

*  Longitude: Floating-point number representing the longitude of the waypoint. 

*  Identifier: Unique identifier (UID) for each waypoint. 

*  Name: Optional name or description for the waypoint. 

A way point shall provide an interface to modify its attribute, except its UID 

*  A link shall have following attributes: 

*  Start Waypoint ID: Unique identifier for the starting waypoint. 

*  End Waypoint ID: Unique identifier for the ending waypoint. 

*  Identifier: Unique identifier (UID) for each link (Start Waypoint ID + End Waypoint ID could be used 
   so each direction of the link could have different UID). 

*  Type: Type of the link, indicating single direction or bidirectional 

A link shall provide an interface to modify its attributes. 

The raster shall have following attributes: 

*  Grid Size: Dimensions of each grid cell, typically in kilometers or nautical miles. 

*  Origin: Reference point (latitude and longitude) for the origin of the raster grid. The origin is the North-West point of the raster 

The raster shall provide an interface to modify its attributes.  

The raster shall provide an interface to return origin (north-west) point of queried grid by X and Y where X and Y are relative the most 
north-west grid of the raster. 

Static Objects
..............

An area may contain various static objects, including wind turbines, platforms, anchoring areas, grounding areas, and wind turbine 
farms, etc. 

All static objects shall have following attributes: 

*  Type: The specific type of the static object, which determines additional attributes and behaviors (e.g., platform, anchoring area, grounding area, wind turbine, wind turbine farm). 

*  Identifier: A unique identifier (UID) for each static object for reference and management. 

*  Name/Description: A name or description providing information about the static object. 

*  Location: 

   *  For point-based objects: Defined by latitude and longitude coordinates. 

   *  For area-based objects: Defined by a set of coordinates that form a polygon, defining the object's boundaries. 

*  Status: The current operational status of the object (active, under maintenance, deactivated, etc.). 

*  Area: The total area covered by the object, automatically calculated for polygon-based objects. For point-based objects, this can 
   be represented by the area they effectively influence or occupy. 

*  Parent Identifier (for grouped objects): For static objects that are part of a larger grouping (e.g., turbines within a wind farm), 
   this identifier links them to their parent object. 

Static objects shall be able to be grouped under a main static object, allowing complex structures such as wind turbine farms to be represented as a 
single entity with different individual components. 

Weather Stations
~~~~~~~~~~~~~~~~ 

TBD - an area contains the probability of an occurrences of a weather condition.  

Traffic
~~~~~~~

A Traffic Database shall consist of one or more traffic network, one or more area, and a unique identifier.  

TBD: indicate ship_links -> this is traffic type route bounded traffic  

Traffic Database shall categorize ships (TBD: static objects) to different types of traffics; bounded and unbounded traffic.  

Each ship can be in one of the categories, either bounded traffic or unbounded traffic. 

Bounded Traffic
...............

Bounded traffic is the collection of ships (TBD: other objects) which are bounded to a traffic network link. 

Bounded traffic shall consist of ships thar are bounded to a traffic network link and those traffic network links themselves.   

Bounded object (ships {and TBD other objects}) shall have following attributes: 

*  Object UID: UID of the bounded object (ship, etc) 

*  Link UID: Traffic Network UID (TBD: this shall reflect the direction of the link) 

*  Lateral Distance: The distance of the bounded object from the reference point (TBD: center) measured lateral to the direction of the link. 

*  Tangent Distance: The distance of the bounded object from the reference point (TBD: center) measured perpendicular to the direction of 
   the link (projected position on link). 

Unbounded Traffic
................. 

The unbounded traffic shall consist of ships which are not bounded to a traffic network link, and those ships are grouped in means of grid 
cells of the traffic network raster. 

An unbounded traffic object (ships {and TBD other objects}) shall have following attributes: 

*  Object UID: UID of the unbounded object (ship, etc.) 

*  Raster Grid Index: Relative grid index to the most north-west grid of the traffic network raster.  

*  X (latitude) index 

*  Y (longitude) index 

Operating Environment
--------------------- 

The software shall be compatible with MS Windows (versions: 10 and 11), Linux distributions (e.g., Ubuntu, CentOS, Fedora) and High-Performance 
Computing (HPC) systems. 

Design and Implementation Constraints
------------------------------------- 

**Programming Language**: Main programming language shall be C++20: 

*  C++20 increases the expressiveness and maintainability of code by introducing modern features such as concepts and coroutines. 

*  C++'s renowned performance capabilities have been further enhanced with C++20 optimizations. 

*  C++20, an ISO/IEC standard, ensures portability and compatibility across platforms and compilers. 

*  C++20 has a wide ecosystem of libraries and tools that support various development tasks and speed up project completion. 

*  Widespread industry adoption of C++20 and long-term support from vendors ensures alignment with best practices and future developments. 

**Build System**: CMake shall be used as the build system: 

*  A build system that generates platform-specific build configurations, allowing developers to build projects across different operating 
   systems and compilers. 

*  Modular design enables efficient management of large-scale projects with multiple components and dependencies.  

*  Integrates well with various external tools and libraries, simplifying the incorporation of third-party dependencies into the build process. 

**Deliverable**: The main deliverable of the software shall be a dynamic library (.so on Unix-like systems, .dll on Windows) to allow more 
flexible updates and patching. The dynamic library(s), with any associated header files and documentation, shall be packaged and distributed 
in a manner that simplifies installation and usage for end-users. 

User Documentation
------------------

See :ref:`User Guide <user_guide>` and :ref:`Tutorials <tutorials>`