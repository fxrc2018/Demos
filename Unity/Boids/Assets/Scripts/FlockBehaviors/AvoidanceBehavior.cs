using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(menuName = "Flock/Behavior/Avoidance")]
public class AvoidanceBehavior : FlockBehavior
{
    public override Vector2 CalculateMove(FlockAgent agent, List<Transform> context, Flock flock)
    {
        if (context.Count == 0)
        {
            return Vector2.zero;
        }

        Vector2 avoidanceMove = Vector2.zero;
        int nAvoid = 0;
        List<Transform> filterContext = filter == null ? context : filter.Filter(agent, context);
        foreach (Transform item in filterContext)
        {
            if(Vector2.SqrMagnitude(item.position - agent.transform.position) < flock.SquareAvoidRadius)
            {
                nAvoid++;
                avoidanceMove += (Vector2)(agent.transform.position - item.position);
            }

        }
        if(nAvoid > 0)
        {
            avoidanceMove /= nAvoid;
        }
        return avoidanceMove;
    }
}
