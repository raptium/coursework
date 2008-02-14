using System;
using System.Collections.Generic;
using System.Text;

namespace rBank
{
    public class Account
    {
        private double balance;
        public double Balance
        {
            set
            {
                balance = value;
            }
            get
            {
                return balance;
            }
        }
        private string name;
        protected string Name
        {
            set
            {
                name = value;
            }
        }
        public void PrintAccount()
        {
            Console.WriteLine(name + ": $" + balance);

        }
        public string getName()
        {
            return name;
        }
        public Account()
        {
            balance = 0;
            name = "";
        }
        public int Deposit(double amount)
        {
            if (amount > 0)
            {
                balance += amount;
                return 0;
            }
            else{
                Console.WriteLine("Deposit cannot be negative!");
                return -1;
            }
        }
        public int Withdraw(double amount)
        {
            if (amount > 0)
            {
                if (amount <= balance)
                {
                    balance += amount;
                    return 0;
                }
                else
                {
                    Console.WriteLine("Not enough balance in the account!");
                    return -1;
                }
            }
            else
            {
                Console.WriteLine("Withdraw cannot be negative!");
                return -1;
            }
        }

        public override string ToString()
        {
            return name + ": $" + balance;
        }
    }

    public class SavingAccount : Account
    {
        public SavingAccount()
        {
            this.Name = "Saving Account";
        }
    }

    public class CurrentAccount : Account
    {
        public CurrentAccount()
        {
            this.Name = "Current Account";
        }
    }

    public class USDAccount : Account
    {
        public USDAccount()
        {
            this.Name = "USD Account";
        }
    }

    
    
}
