using System;
using System.Collections.Generic;

namespace VirtualMachine
{
    // ============================
    // MEMORY
    // ============================
    class Memory
    {
        private string[] memory;

        public Memory(int size)
        {
            memory = new string[size];
        }

        public void Write(int address, string value)
        {
            memory[address] = value;
        }

        public string Read(int address)
        {
            return memory[address];
        }

        public void Dump()
        {
            Console.WriteLine("\n=== MEMORY DUMP ===");
            for (int i = 0; i < memory.Length; i++)
            {
                if (memory[i] != null)
                    Console.WriteLine($"{i}: {memory[i]}");
            }
        }
    }

    // ============================
    // REGISTERS
    // ============================
    class Registers
    {
        // Specific purpose
        public int PC = 0;
        public string IR = "";
        public int MAR = 0;
        public string MBR = "";

        // General purpose
        public int ACC = 0;
        public int AL = 0;
        public int AH = 0;
        public int BL = 0;
        public int BH = 0;

        public void Print()
        {
            Console.WriteLine($"PC: {PC} | IR: {IR} | MAR: {MAR} | MBR: {MBR}");
            Console.WriteLine($"ACC: {ACC} | AL: {AL} | AH: {AH} | BL: {BL} | BH: {BH}");
        }
    }

    // ============================
    // CPU
    // ============================
    class CPU
    {
        private Memory memory;
        private Registers regs;

        public CPU(Memory mem)
        {
            memory = mem;
            regs = new Registers();
        }

        public void Run()
        {
            bool running = true;

            while (running)
            {
                Fetch();
                DecodeExecute();

                if (regs.IR.StartsWith("stop"))
                    running = false;
            }

            Console.WriteLine("\n=== PROGRAM FINISHED ===");
            Console.WriteLine($"Final ACC Value: {regs.ACC}");
        }

        private void Fetch()
        {
            Console.WriteLine("\n--- FETCH ---");

            regs.MAR = regs.PC;
            Console.WriteLine("READ signal activated");

            regs.MBR = memory.Read(regs.MAR);
            regs.IR = regs.MBR;

            regs.PC++;

            regs.Print();
        }

        private void DecodeExecute()
        {
            Console.WriteLine("\n--- DECODE / EXECUTE ---");

            string[] parts = regs.IR.Split(' ');

            switch (parts[0])
            {
                case "start":
                    Console.WriteLine("Program started");
                    break;

                case "move":
                    ExecuteMove(parts);
                    break;

                case "add":
                    ExecuteAdd(parts);
                    break;

                case "sto":
                    ExecuteStore(parts);
                    break;

                case "stop":
                    Console.WriteLine("Program stopped");
                    break;

                default:
                    Console.WriteLine("Unknown instruction");
                    break;
            }

            regs.Print();
        }

        private void ExecuteMove(string[] parts)
        {
            string reg = parts[1];
            int value = int.Parse(parts[2]);

            SetRegister(reg, value);
        }

        private void ExecuteAdd(string[] parts)
        {
            int val1 = GetRegister(parts[1]);
            int val2 = GetRegister(parts[2]);

            regs.ACC = val1 + val2;
        }

        private void ExecuteStore(string[] parts)
        {
            int address = int.Parse(parts[1]);

            Console.WriteLine("WRITE signal activated");
            memory.Write(address, regs.ACC.ToString());
        }

        private int GetRegister(string name)
        {
            return name switch
            {
                "AL" => regs.AL,
                "AH" => regs.AH,
                "BL" => regs.BL,
                "BH" => regs.BH,
                "ACC" => regs.ACC,
                _ => 0
            };
        }

        private void SetRegister(string name, int value)
        {
            switch (name)
            {
                case "AL": regs.AL = value; break;
                case "AH": regs.AH = value; break;
                case "BL": regs.BL = value; break;
                case "BH": regs.BH = value; break;
                case "ACC": regs.ACC = value; break;
            }
        }
    }

    // ============================
    // LOADER
    // ============================
    class Loader
    {
        public void LoadProgram(Memory memory)
        {
            memory.Write(0, "start");
            memory.Write(1, "move AL 10");
            memory.Write(2, "move BL 20");
            memory.Write(3, "add AL BL");
            memory.Write(4, "sto 8");
            memory.Write(5, "stop");
        }
    }

    // ============================
    // VIRTUAL MACHINE
    // ============================
    class VirtualMachine
    {
        private Memory memory;
        private CPU cpu;
        private Loader loader;

        public VirtualMachine()
        {
            memory = new Memory(20);
            loader = new Loader();
            cpu = new CPU(memory);
        }

        public void Start()
        {
            loader.LoadProgram(memory);
            cpu.Run();
            memory.Dump();
        }
    }

    // ============================
    // MAIN
    // ============================
    class Program
    {
        static void Main(string[] args)
        {
            VirtualMachine vm = new VirtualMachine();
            vm.Start();
        }
    }
}

