package se.umu.cs.backend.core;

public enum Stone {
    BLACK("Black"),
    WHITE("White"),
    UNUSED("Unused"),
    VALID("Valid"),
    INVALID("Invalid");

    private final String displayName;

    Stone(String displayName) {
        this.displayName = displayName;
    }

    public Stone opposite() {
        if (this == UNUSED || this == INVALID || this == VALID) {
            return this;
        }
        return this == BLACK ? WHITE : BLACK;
    }

    public String getDisplayName() {
        return displayName;
    }
}
