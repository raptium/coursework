using System;
using System.Collections.Generic;
using System.Text;
using Customer;

namespace bankApp
{
    class bankApp
    {
        static void Main(string[] args)
        {
            Customer.Customer[] customers = new Customer.Customer[2];
            customers[0] = new Customer.Customer("James Bond", 0);
            customers[1] = new Customer.Customer("Goldfinger", 1);

            for (int i = 0; i < customers.Length; i++)
                customers[i].PrintCustomer();

            customers[0].Deposite(0, 100);
            customers[0].Deposite(1, 300);

            customers[1].Deposite(0, 20000);
            customers[1].Deposite(1, 50000);

            for (int i = 0; i < customers.Length; i++)
                customers[i].PrintCustomer();
        }
    }
}
