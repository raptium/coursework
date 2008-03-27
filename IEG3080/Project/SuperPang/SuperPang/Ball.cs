using System;
using System.Drawing;
using System.Collections.Generic;
using System.Text;

namespace SuperPang
{
    public class Ball : GameObject
    {

        private int type;
        private Boolean destroyed;

        public Ball(Rectangle initLocationInfo,
            double init_Velocity_x,
            double init_Velocity_y, Model model)
            :
            base(initLocationInfo,
            init_Velocity_x,
            init_Velocity_y,
            model)
        {
            type = 3;
            image = Properties.Resources.MacBook_Pro;
            
        }

        public Ball(Rectangle initLocationInfo,
            double init_Velocity_x,
            double init_Velocity_y, Model model, int t)
            :
            base(initLocationInfo,
            init_Velocity_x,
            init_Velocity_y,
            model)
        {
            type = t;
            switch(t)
            {
                case 3:
                    image = Properties.Resources.MacBook_Pro;
                    break;
                case 2:
                    locationInfo.Width = 60;
                    locationInfo.Height = 60;
                    image = Properties.Resources.MacBook_White;
                    break;
                case 1:
                    locationInfo.Width = 36;
                    locationInfo.Height = 36;
                    image = Properties.Resources.MacBook_Air;
                    break;
            }
            
        }


        public Boolean Destroyed{
            get { return destroyed; }
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

        public List<Ball> Split()
        {
            List<Ball> balls = new List<Ball>();

            destroyed = true;
            if (type > 1)
            {
                balls.Add(new Ball(locationInfo, 10, 0, model, type - 1));
                balls.Add(new Ball(locationInfo, -10, 0, model, type - 1));
            }

            return balls;
        }
    }
}
