protected:
    char *block;
    bool owned;
    static const hel headerSizes[];
    static const hel staticSizes[];

    hel headerSize(rid id);
};

