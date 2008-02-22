using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using System.ComponentModel;
using System.Collections.Generic;
using System.Text;

namespace SuperPang
{
    class GameView : View
    {
        public GameView(Model model, Controller controller)
            : base(model, controller)
        {
            this.SetStyle(ControlStyles.AllPaintingInWmPaint |
                ControlStyles.UserPaint |
                ControlStyles.DoubleBuffer, true);
        }

        public override void update(Model model)
        {
            Invalidate();
            model.getHero().move(800, 550);
            model.getBall().move(800, 420);
            makeHero(model.getHero());
            makeBall(model.getBall());
        }

        private void makeHero(Hero hero)
        {
            Bitmap img = Properties.Resources.doraemon;
            g.DrawImage(img, hero.getLocationInfo().X, hero.getLocationInfo().Y);
        }

        private void makeBall(Ball ball)
        {
            Bitmap img = Properties.Resources.chara_haro_a;
            g.DrawImage(img, ball.getLocationInfo().X, ball.getLocationInfo().Y);
        }
    }
}
