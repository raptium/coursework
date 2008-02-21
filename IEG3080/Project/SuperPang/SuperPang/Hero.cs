using System;
using System.Drawing;
using System.Collections.Generic;
using System.Text;

namespace SuperPang
{
    public class Hero : GameObject
    {

        public Hero(Rectangle initLocationInfo,double initOrientation,double initVelocity_x,double init_Velocity_y,Model model) : base (initLocationInfo,initOrientation,initVelocity_x,init_Velocity_y,model)
		{
			
		}
        public override void move(int max_height, int max_width)
        {

        }
    }
}
