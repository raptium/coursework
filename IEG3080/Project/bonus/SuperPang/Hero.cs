using System;
using System.Drawing;
using System.Collections.Generic;
using System.Text;

namespace SuperPang
{
    public class Hero : GameObject
    {

        public Hero(Rectangle initLocationInfo,
            double initVelocity_x,
            double init_Velocity_y,Model model) :
            base (initLocationInfo,
            initVelocity_x,
            init_Velocity_y,
            model)
		{
            image = Properties.Resources.Hero;
            imgSrc = "Resources\\Hero.png";
		}

        public override void move(int max_width, int max_height)
        {
            locationInfo.X += (int)velocity_x;
            locationInfo.Y += (int)velocity_y;
            velocity_x *= 0.5;

            //velocity_x *= 0.95;
            if(velocity_y != 0)
                velocity_y += 3;

            if (this.locationInfo.Y <= 0)
            {
                this.locationInfo.Y = 1;
            }
            else if (this.locationInfo.Y >= max_height)
            {
                this.locationInfo.Y = max_height - 1;
                velocity_y = 0;
            }

            if (this.locationInfo.X <= 0)
                this.locationInfo.X = 1;
            else if (this.locationInfo.X >= max_width)
                this.locationInfo.X = max_width - 1;
        }

        public void moveLeft()
        {
            this.velocity_x = -15;
        }

        public void moveRight()
        {
            this.velocity_x = 15;
        }

        public void stop()
        {
            this.velocity_x = 0;
        }

        public void jump()
        {
            if(this.velocity_y == 0)
             this.velocity_y = -20;
        }
    }
}
