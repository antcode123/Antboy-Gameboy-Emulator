

typedef unsigned char U8;
typedef unsigned short U16;


namespace Hardware {
    class MBC {
    public:
        bool is_ram_enabled;
        bool is_rom_bank_mode;
        int type;
        int rom_bank;
        int ram_bank;
        int total_rom_banks;

        MBC(int _type);
        virtual void write(U16 address, U8 u8);
        virtual void modify_rom_bank(U16 address, U8& temporary_rom_bank);
    };


    class MBC1 : public MBC {
    public:
        MBC1();
        void write(U16 address, U8 u8) override;
        void modify_rom_bank(U16 address, U8& temporary_rom_bank);
    };


    class MBC2 : public MBC {
    public:
        MBC2();
        void write(U16 address, U8 u8) override;
    };
}