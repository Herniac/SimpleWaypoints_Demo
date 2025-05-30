<p align="center" width="100%">
    <img src="https://raw.githubusercontent.com/Herniac/SimpleWaypoints_Demo/refs/heads/main/Plugins/SimpleWaypoints/Resources/Icon256.png"> 
</p>

## **SimpleWaypoints** - Intelligent destination selection

SimpleWaypoints enables straightforward AI pathing using modular, logic-driven waypoint systems—with almost no setup required. Built on top of **ExtraLogic** (**required**), the plugin introduces three core components: WaypointGraph, Waypoint, and WaypointFollowerComponent.

## Key Features:

🚩 **WaypointGraph:** Acts as the parent actor for organizing waypoint nodes. You can easily add waypoints of a defined class to the graph using the CreateWaypoint button. Pass the graph to the WaypointFollowerComponent to define the scope of waypoint selection for the AI.
<p align="center" width="100%">
  <img src="https://raw.githubusercontent.com/Herniac/SimpleWaypoints_Demo/heads/main/Resources/Img_WaypointGraph.png"> 
</p>

🚩 **WaypointFollowerComponent:** Can be attached either to AIController or Character. The component handles waypoint selection, history tracking and cooldown logic when destinations are unreachable. It also allows you to decide whether AI should loop the path or stop movement upon reaching the final waypoint.
<p align="center" width="100%">
  <img src="https://raw.githubusercontent.com/Herniac/SimpleWaypoints_Demo/heads/main/Resources/Img_WaypointFollower.png"> 
</p>

🚩 **Waypoint:** Stores information about its possible destinations, including weighted probabilities for selection. They offer a handful of parameters such as cooldown time when unreachable, dynamic behaviors to execute upon arrival, and conditions that must be met for selection.
<p align="center" width="100%">
  <img src="https://raw.githubusercontent.com/Herniac/SimpleWaypoints_Demo/heads/main/Resources/Img_Waypoint.png"> 
</p>

🚩 **Behavior Tree Nodes:** For convenience, this plugin includes three main BT nodes:

- MoveToWaypoint (BTTask) — moves the AI to a waypoint, marks it as reached on success, and applies cooldown on failure.

- SelectWaypoint (BTService) — handles selecting the next waypoint during behavior execution.

- ContainsDynamicBehavior (BTDecorator) — checks if dynamic behavior should be injected at a given moment.

A sample behavior tree demonstrating these nodes is available in the demo project.
