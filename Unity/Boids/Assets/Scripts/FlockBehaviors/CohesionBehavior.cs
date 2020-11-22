using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(menuName = "Flock/Behavior/Cohesion")]
public class CohesionBehavior : FlockBehavior
{
    Vector2 currentVelocity;
    public float agentSmoothTime = 0.5f;

    public override Vector2 CalculateMove(FlockAgent agent, List<Transform> context, Flock flock)
    {
        //if no neighbors, return no adjustment
        if(context.Count == 0)
        {
            return Vector2.zero;
        }
        //找到邻居的中点，尝试到这里去
        Vector2 cohesionMove = Vector2.zero;
        List<Transform> filterContext = filter == null ? context : filter.Filter(agent, context);
        foreach (Transform item in filterContext)
        {
            cohesionMove += (Vector2)item.position;

        }
        cohesionMove /= context.Count;

        //create offset 
        cohesionMove -= (Vector2)agent.transform.position;
        cohesionMove = Vector2.SmoothDamp(agent.transform.up, cohesionMove, ref currentVelocity, agentSmoothTime);
        return cohesionMove;
    }
}
