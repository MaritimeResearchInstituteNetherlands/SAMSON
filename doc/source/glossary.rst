.. _`glossary`:

********
Glossary
********

.. warning::

   Under Development

The terminology in the SAMSON documentation is listed in this glossary.  

.. glossary::

    Area
        An area holds traffic objects, static objects, weather stations and ERTVs.        

    Collision
        Collision is defined as the intersection of two objects.
        
    Danger miles
        The danger miles are the implementation of the exposure between a ship and a static object.    
        
    Drift
        A drifting ship is caused by an engine failure.     

    Exposure between ship and ship
        An exposure between two ships occurs when the ship contours overlap eachother. This type of exposure is also called 
        collison candidate.    

    Exposure between ship and static object
        An exposure between a ship and a static object occurs when the ship contour overlaps the static object. The ship contour 
        is represented by a rectangular shape. The static object domain is represented by either a polyline or a polygon or a 
        combination of these.         
        
    Link
        A link is represented by a vector. It is the simplified representation of a shipping lane. Only bounded traffic that fulfils 
        the set criteria is assigned to a link. If not, the ship is tagged as unassigned bounded traffic and added to the raster.
        
    Measures implemented
        Measures result in a reduction of the probability of occurence, but their implementation might involve cost.    

    Pilot
        A pilot is a mariner who has specific knowledge of an often dangerous or congested waterway, such as harbors or river mouths. 
        Maritime pilots know local details such as depth, currents, and hazards. They board and temporarily join the crew to safely 
        guide the ship's passage, so they must also have expertise in handling ships of all types and sizes.     
    
    Ram
        A ramming ship is caused by a navigational error.    

    Traffic
        The ships moving in an area during a specified time period are denoted as traffic.

    Ship
        Ships are classified in ship types. There are multiple sizes within each type. Each combination of type and size is acccompanied 
        with a set of main particulars and features. The main particulars represent the ship's main dimensions, which are applied in the 
        geometrical exposure computations. The features represent for example the cargo type or the number of passengers on board.          
        
    Static object 
        An static object in an area is represented by geometry and might be potential harmful for a drifting or ramming ship.   

    Traffic objects
        Traffic objects consist of waypoints, links and cells. 

    Traffic separation scheme
        A routeing measure aimed at the separation of opposing streams of traffic by appropriate means and by the establishment of traffic lanes.       

    Waypoint
        A waypoint is a vertex and represents the end points of a link.
        
    Weather station
        The weather station is represented by a point in an area and holds the probability for wind, waves and/or current.    