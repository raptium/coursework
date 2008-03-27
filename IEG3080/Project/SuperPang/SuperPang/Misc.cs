using System;
using System.Drawing;
using System.Collections.Generic;
using System.Text;

namespace SuperPang
{
    public class Misc : GameObject
    {
        public Misc(Rectangle initLocationInfo,
            double init_Velocity_x,
            double init_Velocity_y, Model model, Image image)
            :
            base(initLocationInfo,
            init_Velocity_x,
            init_Velocity_y,
            model,
            image)
        {
            
        }

        public override void move(int max_width, int max_height)
        {
        }
    }
}
