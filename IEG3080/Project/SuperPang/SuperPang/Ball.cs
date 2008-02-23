using System;
using System.Drawing;
using System.Collections.Generic;
using System.Text;

namespace SuperPang
{
    public class Ball : GameObject
    {

        public Ball(Rectangle initLocationInfo,
            double initOrientation,
            double init_Velocity_x,
            double init_Velocity_y, Model model)
            :
            base(initLocationInfo,
            initOrientation,
            init_Velocity_x,
            init_Velocity_y,
            model)
        {


        }

        public override void move(int max_width, int max_height)
        {
            locationInfo.X += (int)velocity_x;
            locationInfo.Y += (int)velocity_y;
            velocity_y += 0.04 * 40;

            if (this.locationInfo.Y <= 0)
            {
                this.locationInfo.Y = 0;
                velocity_y = -0.91 * velocity_y;
            }
            if (this.locationInfo.Y >= max_height)
            {
                this.locationInfo.Y = max_height;
                velocity_y = -0.91 * velocity_y;
            }
            if (this.locationInfo.X <= 0)
            {
                this.locationInfo.X = 0;
                velocity_x = -velocity_x;
            }
            if (this.locationInfo.X >= max_width)
            {
                this.locationInfo.X = max_width;
                velocity_x = -velocity_x;
            }
        }
    }
}
