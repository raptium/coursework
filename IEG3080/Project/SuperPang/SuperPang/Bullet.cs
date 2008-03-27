using System;
using System.Drawing;
using System.Collections.Generic;
using System.Text;

namespace SuperPang
{
    public class Bullet : GameObject
    {

        private Boolean destroyed;

        public Bullet(Rectangle initLocationInfo,
            double init_Velocity_x,
            double init_Velocity_y, Model model)
            :
            base(initLocationInfo,
            init_Velocity_x,
            init_Velocity_y,
            model)
        {
            image = Properties.Resources.nano;
            locationInfo.Width = 12;
            locationInfo.Height = 12;
            destroyed = false;
        }


        public Boolean Destroyed{
            get { return destroyed; }
            set { destroyed = value; }
        }


        public override void move(int max_width, int max_height)
        {
            locationInfo.X += (int)velocity_x;
            locationInfo.Y += (int)velocity_y;

            if (this.locationInfo.Y <= 0)
            {
                destroyed = true;
            }
            if (this.locationInfo.Y >= max_height)
            {
                destroyed = true;
            }
            if (this.locationInfo.X <= 0)
            {
                destroyed = true;
            }
            if (this.locationInfo.X >= max_width)
            {
                destroyed = true;
            }
        }
    }
}
