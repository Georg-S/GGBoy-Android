package com.example.ggboy;

public enum GGBoyButton
{
    A(0),
    B(1),
    START(2),
    SELECT(3),
    LEFT(4),
    RIGHT(5),
    UP(6),
    DOWN(7);

    private final int value;

    GGBoyButton(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }
}
