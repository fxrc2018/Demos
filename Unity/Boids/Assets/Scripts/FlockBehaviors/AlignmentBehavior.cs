using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[CreateAssetMenu(menuName = "Flock/Behavior/Alignment")]
public class AlignmentBehavior : FlockBehavior
{
    public override Vector2 CalculateMove(FlockAgent agent, List<Transform> context, Flock flock)
    {
        if (context.Count == 0)
        {
            return agent.transform.up;
        }
        Vector2 alignmentMove = Vector2.zero;
        List<Transform> filterContext = filter == null ? context : filter.Filter(agent ,context);
        foreach (Transform item in filterContext)
        {
            alignmentMove += (Vector2)item.transform.up;

        }
        alignmentMove /= context.Count;
        return alignmentMove;
    }
}
