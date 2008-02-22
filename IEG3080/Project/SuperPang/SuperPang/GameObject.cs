using System;
using System.Drawing;
using System.Collections.Generic;
using System.Text;

namespace SuperPang
{
    public abstract class GameObject
    {
        protected Rectangle locationInfo;
		protected double orientation = 0;
		protected double velocity_x;
		protected double velocity_y;
		protected Model model = null;

		protected Random ran_gen = new Random();


		public GameObject(Rectangle initLocationInfo,
            double initOrientation,
            double initVelocity_x,
            double initVelocity_y,
            Model model) :
            base()
		{
			locationInfo = initLocationInfo;
			orientation = initOrientation;
			velocity_x = initVelocity_x;			
			velocity_y = initVelocity_y;			
			this.model = model;
		}

		public Rectangle getLocationInfo()
		{
			return locationInfo;
		}

		public double getOrientation()
		{
			return orientation;
		}

		public bool checkCollide(Rectangle other) 
		{
			return this.locationInfo.IntersectsWith(other);		
		}

		public abstract void move(int max_width, int max_height);

    }
}
