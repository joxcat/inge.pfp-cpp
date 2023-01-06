// Soure: https://stackoverflow.com/questions/4969233/how-to-check-if-enum-value-is-valid

template<typename EnumType, EnumType... Values> class EnumCheck;

template<typename EnumType> class EnumCheck<EnumType>
{
public:
    template<typename IntType>
    static bool constexpr is_value(IntType) { return false; }
};

template<typename EnumType, EnumType V, EnumType... Next>
class EnumCheck<EnumType, V, Next...> : private EnumCheck<EnumType, Next...>
{
    using super = EnumCheck<EnumType, Next...>;

public:
    template<typename IntType>
    static bool constexpr is_value(IntType v)
    {
        return v == static_cast<IntType>(V) || super::is_value(v);
    }
};
