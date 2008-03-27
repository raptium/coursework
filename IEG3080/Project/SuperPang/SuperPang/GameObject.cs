using System;
using System.Drawing;
using System.Collections.Generic;
using System.Text;

namespace SuperPang
{
    public abstract class GameObject
    {
        protected Rectangle locationInfo;
		protected double velocity_x;
		protected double velocity_y;
		protected Model model = null;
        protected Image image;

		protected Random ran_gen = new Random();


		public GameObject(Rectangle initLocationInfo,
            double initVelocity_x,
            double initVelocity_y,
            Model model) :
            base()
		{
			locationInfo = initLocationInfo;
			velocity_x = initVelocity_x;			
			velocity_y = initVelocity_y;			
			this.model = model;
		}

        public GameObject(Rectangle initLocationInfo,
            double initVelocity_x,
            double initVelocity_y,
            Model model, Image img) :
            base()
        {
            locationInfo = initLocationInfo;
            velocity_x = initVelocity_x;
            velocity_y = initVelocity_y;
            this.model = model;
            image = img;
        }

		public Rectangle Position
		{
            get { return locationInfo; }
		}

		public bool checkCollide(Rectangle other) 
		{
			return this.locationInfo.IntersectsWith(other);
		}

		public abstract void move(int max_width, int max_height);
        public Image Image
        {
            get { return image; }
        }

    }
}
