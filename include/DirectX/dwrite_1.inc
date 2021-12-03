ifndef DWRITE_1_H_INCLUDED
define DWRITE_1_H_INCLUDED

include dwrite.inc

.enum DWRITE_PANOSE_FAMILY {
    DWRITE_PANOSE_FAMILY_ANY = 0,
    DWRITE_PANOSE_FAMILY_NO_FIT = 1,
    DWRITE_PANOSE_FAMILY_TEXT_DISPLAY = 2,
    DWRITE_PANOSE_FAMILY_SCRIPT = 3,
    DWRITE_PANOSE_FAMILY_DECORATIVE = 4,
    DWRITE_PANOSE_FAMILY_SYMBOL = 5,
    DWRITE_PANOSE_FAMILY_PICTORIAL = DWRITE_PANOSE_FAMILY_SYMBOL
    }

.enum DWRITE_PANOSE_SERIF_STYLE {
    DWRITE_PANOSE_SERIF_STYLE_ANY = 0,
    DWRITE_PANOSE_SERIF_STYLE_NO_FIT = 1,
    DWRITE_PANOSE_SERIF_STYLE_COVE = 2,
    DWRITE_PANOSE_SERIF_STYLE_OBTUSE_COVE = 3,
    DWRITE_PANOSE_SERIF_STYLE_SQUARE_COVE = 4,
    DWRITE_PANOSE_SERIF_STYLE_OBTUSE_SQUARE_COVE = 5,
    DWRITE_PANOSE_SERIF_STYLE_SQUARE = 6,
    DWRITE_PANOSE_SERIF_STYLE_THIN = 7,
    DWRITE_PANOSE_SERIF_STYLE_OVAL = 8,
    DWRITE_PANOSE_SERIF_STYLE_EXAGGERATED = 9,
    DWRITE_PANOSE_SERIF_STYLE_TRIANGLE = 10,
    DWRITE_PANOSE_SERIF_STYLE_NORMAL_SANS = 11,
    DWRITE_PANOSE_SERIF_STYLE_OBTUSE_SANS = 12,
    DWRITE_PANOSE_SERIF_STYLE_PERPENDICULAR_SANS = 13,
    DWRITE_PANOSE_SERIF_STYLE_FLARED = 14,
    DWRITE_PANOSE_SERIF_STYLE_ROUNDED = 15,
    DWRITE_PANOSE_SERIF_STYLE_SCRIPT = 16,
    DWRITE_PANOSE_SERIF_STYLE_PERP_SANS = DWRITE_PANOSE_SERIF_STYLE_PERPENDICULAR_SANS,
    DWRITE_PANOSE_SERIF_STYLE_BONE = DWRITE_PANOSE_SERIF_STYLE_OVAL
    }

.enum DWRITE_PANOSE_WEIGHT {
    DWRITE_PANOSE_WEIGHT_ANY = 0,
    DWRITE_PANOSE_WEIGHT_NO_FIT = 1,
    DWRITE_PANOSE_WEIGHT_VERY_LIGHT = 2,
    DWRITE_PANOSE_WEIGHT_LIGHT = 3,
    DWRITE_PANOSE_WEIGHT_THIN = 4,
    DWRITE_PANOSE_WEIGHT_BOOK = 5,
    DWRITE_PANOSE_WEIGHT_MEDIUM = 6,
    DWRITE_PANOSE_WEIGHT_DEMI = 7,
    DWRITE_PANOSE_WEIGHT_BOLD = 8,
    DWRITE_PANOSE_WEIGHT_HEAVY = 9,
    DWRITE_PANOSE_WEIGHT_BLACK = 10,
    DWRITE_PANOSE_WEIGHT_EXTRA_BLACK = 11,
    DWRITE_PANOSE_WEIGHT_NORD = DWRITE_PANOSE_WEIGHT_EXTRA_BLACK
    }

.enum DWRITE_PANOSE_PROPORTION {
    DWRITE_PANOSE_PROPORTION_ANY = 0,
    DWRITE_PANOSE_PROPORTION_NO_FIT = 1,
    DWRITE_PANOSE_PROPORTION_OLD_STYLE = 2,
    DWRITE_PANOSE_PROPORTION_MODERN = 3,
    DWRITE_PANOSE_PROPORTION_EVEN_WIDTH = 4,
    DWRITE_PANOSE_PROPORTION_EXPANDED = 5,
    DWRITE_PANOSE_PROPORTION_CONDENSED = 6,
    DWRITE_PANOSE_PROPORTION_VERY_EXPANDED = 7,
    DWRITE_PANOSE_PROPORTION_VERY_CONDENSED = 8,
    DWRITE_PANOSE_PROPORTION_MONOSPACED = 9
    }

.enum DWRITE_PANOSE_CONTRAST {
    DWRITE_PANOSE_CONTRAST_ANY = 0,
    DWRITE_PANOSE_CONTRAST_NO_FIT = 1,
    DWRITE_PANOSE_CONTRAST_NONE = 2,
    DWRITE_PANOSE_CONTRAST_VERY_LOW = 3,
    DWRITE_PANOSE_CONTRAST_LOW = 4,
    DWRITE_PANOSE_CONTRAST_MEDIUM_LOW = 5,
    DWRITE_PANOSE_CONTRAST_MEDIUM = 6,
    DWRITE_PANOSE_CONTRAST_MEDIUM_HIGH = 7,
    DWRITE_PANOSE_CONTRAST_HIGH = 8,
    DWRITE_PANOSE_CONTRAST_VERY_HIGH = 9,
    DWRITE_PANOSE_CONTRAST_HORIZONTAL_LOW = 10,
    DWRITE_PANOSE_CONTRAST_HORIZONTAL_MEDIUM = 11,
    DWRITE_PANOSE_CONTRAST_HORIZONTAL_HIGH = 12,
    DWRITE_PANOSE_CONTRAST_BROKEN = 13
    }

.enum DWRITE_PANOSE_STROKE_VARIATION {
    DWRITE_PANOSE_STROKE_VARIATION_ANY = 0,
    DWRITE_PANOSE_STROKE_VARIATION_NO_FIT = 1,
    DWRITE_PANOSE_STROKE_VARIATION_NO_VARIATION = 2,
    DWRITE_PANOSE_STROKE_VARIATION_GRADUAL_DIAGONAL = 3,
    DWRITE_PANOSE_STROKE_VARIATION_GRADUAL_TRANSITIONAL = 4,
    DWRITE_PANOSE_STROKE_VARIATION_GRADUAL_VERTICAL = 5,
    DWRITE_PANOSE_STROKE_VARIATION_GRADUAL_HORIZONTAL = 6,
    DWRITE_PANOSE_STROKE_VARIATION_RAPID_VERTICAL = 7,
    DWRITE_PANOSE_STROKE_VARIATION_RAPID_HORIZONTAL = 8,
    DWRITE_PANOSE_STROKE_VARIATION_INSTANT_VERTICAL = 9,
    DWRITE_PANOSE_STROKE_VARIATION_INSTANT_HORIZONTAL = 10
    }

.enum DWRITE_PANOSE_ARM_STYLE {
    DWRITE_PANOSE_ARM_STYLE_ANY = 0,
    DWRITE_PANOSE_ARM_STYLE_NO_FIT = 1,
    DWRITE_PANOSE_ARM_STYLE_STRAIGHT_ARMS_HORIZONTAL = 2,
    DWRITE_PANOSE_ARM_STYLE_STRAIGHT_ARMS_WEDGE = 3,
    DWRITE_PANOSE_ARM_STYLE_STRAIGHT_ARMS_VERTICAL = 4,
    DWRITE_PANOSE_ARM_STYLE_STRAIGHT_ARMS_SINGLE_SERIF = 5,
    DWRITE_PANOSE_ARM_STYLE_STRAIGHT_ARMS_DOUBLE_SERIF = 6,
    DWRITE_PANOSE_ARM_STYLE_NONSTRAIGHT_ARMS_HORIZONTAL = 7,
    DWRITE_PANOSE_ARM_STYLE_NONSTRAIGHT_ARMS_WEDGE = 8,
    DWRITE_PANOSE_ARM_STYLE_NONSTRAIGHT_ARMS_VERTICAL = 9,
    DWRITE_PANOSE_ARM_STYLE_NONSTRAIGHT_ARMS_SINGLE_SERIF = 10,
    DWRITE_PANOSE_ARM_STYLE_NONSTRAIGHT_ARMS_DOUBLE_SERIF = 11,
    DWRITE_PANOSE_ARM_STYLE_STRAIGHT_ARMS_HORZ = DWRITE_PANOSE_ARM_STYLE_STRAIGHT_ARMS_HORIZONTAL,
    DWRITE_PANOSE_ARM_STYLE_STRAIGHT_ARMS_VERT = DWRITE_PANOSE_ARM_STYLE_STRAIGHT_ARMS_VERTICAL,
    DWRITE_PANOSE_ARM_STYLE_BENT_ARMS_HORZ = DWRITE_PANOSE_ARM_STYLE_NONSTRAIGHT_ARMS_HORIZONTAL,
    DWRITE_PANOSE_ARM_STYLE_BENT_ARMS_WEDGE = DWRITE_PANOSE_ARM_STYLE_NONSTRAIGHT_ARMS_WEDGE,
    DWRITE_PANOSE_ARM_STYLE_BENT_ARMS_VERT = DWRITE_PANOSE_ARM_STYLE_NONSTRAIGHT_ARMS_VERTICAL,
    DWRITE_PANOSE_ARM_STYLE_BENT_ARMS_SINGLE_SERIF = DWRITE_PANOSE_ARM_STYLE_NONSTRAIGHT_ARMS_SINGLE_SERIF,
    DWRITE_PANOSE_ARM_STYLE_BENT_ARMS_DOUBLE_SERIF = DWRITE_PANOSE_ARM_STYLE_NONSTRAIGHT_ARMS_DOUBLE_SERIF
    }

.enum DWRITE_PANOSE_LETTERFORM {
    DWRITE_PANOSE_LETTERFORM_ANY = 0,
    DWRITE_PANOSE_LETTERFORM_NO_FIT = 1,
    DWRITE_PANOSE_LETTERFORM_NORMAL_CONTACT = 2,
    DWRITE_PANOSE_LETTERFORM_NORMAL_WEIGHTED = 3,
    DWRITE_PANOSE_LETTERFORM_NORMAL_BOXED = 4,
    DWRITE_PANOSE_LETTERFORM_NORMAL_FLATTENED = 5,
    DWRITE_PANOSE_LETTERFORM_NORMAL_ROUNDED = 6,
    DWRITE_PANOSE_LETTERFORM_NORMAL_OFF_CENTER = 7,
    DWRITE_PANOSE_LETTERFORM_NORMAL_SQUARE = 8,
    DWRITE_PANOSE_LETTERFORM_OBLIQUE_CONTACT = 9,
    DWRITE_PANOSE_LETTERFORM_OBLIQUE_WEIGHTED = 10,
    DWRITE_PANOSE_LETTERFORM_OBLIQUE_BOXED = 11,
    DWRITE_PANOSE_LETTERFORM_OBLIQUE_FLATTENED = 12,
    DWRITE_PANOSE_LETTERFORM_OBLIQUE_ROUNDED = 13,
    DWRITE_PANOSE_LETTERFORM_OBLIQUE_OFF_CENTER = 14,
    DWRITE_PANOSE_LETTERFORM_OBLIQUE_SQUARE = 15
    }

.enum DWRITE_PANOSE_MIDLINE {
    DWRITE_PANOSE_MIDLINE_ANY = 0,
    DWRITE_PANOSE_MIDLINE_NO_FIT = 1,
    DWRITE_PANOSE_MIDLINE_STANDARD_TRIMMED = 2,
    DWRITE_PANOSE_MIDLINE_STANDARD_POINTED = 3,
    DWRITE_PANOSE_MIDLINE_STANDARD_SERIFED = 4,
    DWRITE_PANOSE_MIDLINE_HIGH_TRIMMED = 5,
    DWRITE_PANOSE_MIDLINE_HIGH_POINTED = 6,
    DWRITE_PANOSE_MIDLINE_HIGH_SERIFED = 7,
    DWRITE_PANOSE_MIDLINE_CONSTANT_TRIMMED = 8,
    DWRITE_PANOSE_MIDLINE_CONSTANT_POINTED = 9,
    DWRITE_PANOSE_MIDLINE_CONSTANT_SERIFED = 10,
    DWRITE_PANOSE_MIDLINE_LOW_TRIMMED = 11,
    DWRITE_PANOSE_MIDLINE_LOW_POINTED = 12,
    DWRITE_PANOSE_MIDLINE_LOW_SERIFED = 13
    }

.enum DWRITE_PANOSE_XHEIGHT {
    DWRITE_PANOSE_XHEIGHT_ANY = 0,
    DWRITE_PANOSE_XHEIGHT_NO_FIT = 1,
    DWRITE_PANOSE_XHEIGHT_CONSTANT_SMALL = 2,
    DWRITE_PANOSE_XHEIGHT_CONSTANT_STANDARD = 3,
    DWRITE_PANOSE_XHEIGHT_CONSTANT_LARGE = 4,
    DWRITE_PANOSE_XHEIGHT_DUCKING_SMALL = 5,
    DWRITE_PANOSE_XHEIGHT_DUCKING_STANDARD = 6,
    DWRITE_PANOSE_XHEIGHT_DUCKING_LARGE = 7,
    DWRITE_PANOSE_XHEIGHT_CONSTANT_STD = DWRITE_PANOSE_XHEIGHT_CONSTANT_STANDARD,
    DWRITE_PANOSE_XHEIGHT_DUCKING_STD = DWRITE_PANOSE_XHEIGHT_DUCKING_STANDARD
    }

.enum DWRITE_PANOSE_TOOL_KIND {
    DWRITE_PANOSE_TOOL_KIND_ANY = 0,
    DWRITE_PANOSE_TOOL_KIND_NO_FIT = 1,
    DWRITE_PANOSE_TOOL_KIND_FLAT_NIB = 2,
    DWRITE_PANOSE_TOOL_KIND_PRESSURE_POINT = 3,
    DWRITE_PANOSE_TOOL_KIND_ENGRAVED = 4,
    DWRITE_PANOSE_TOOL_KIND_BALL = 5,
    DWRITE_PANOSE_TOOL_KIND_BRUSH = 6,
    DWRITE_PANOSE_TOOL_KIND_ROUGH = 7,
    DWRITE_PANOSE_TOOL_KIND_FELT_PEN_BRUSH_TIP = 8,
    DWRITE_PANOSE_TOOL_KIND_WILD_BRUSH = 9
    }

.enum DWRITE_PANOSE_SPACING {
    DWRITE_PANOSE_SPACING_ANY = 0,
    DWRITE_PANOSE_SPACING_NO_FIT = 1,
    DWRITE_PANOSE_SPACING_PROPORTIONAL_SPACED = 2,
    DWRITE_PANOSE_SPACING_MONOSPACED = 3,
    }

.enum DWRITE_PANOSE_ASPECT_RATIO {
    DWRITE_PANOSE_ASPECT_RATIO_ANY = 0,
    DWRITE_PANOSE_ASPECT_RATIO_NO_FIT = 1,
    DWRITE_PANOSE_ASPECT_RATIO_VERY_CONDENSED = 2,
    DWRITE_PANOSE_ASPECT_RATIO_CONDENSED = 3,
    DWRITE_PANOSE_ASPECT_RATIO_NORMAL = 4,
    DWRITE_PANOSE_ASPECT_RATIO_EXPANDED = 5,
    DWRITE_PANOSE_ASPECT_RATIO_VERY_EXPANDED = 6
    }

.enum DWRITE_PANOSE_SCRIPT_TOPOLOGY {
    DWRITE_PANOSE_SCRIPT_TOPOLOGY_ANY = 0,
    DWRITE_PANOSE_SCRIPT_TOPOLOGY_NO_FIT = 1,
    DWRITE_PANOSE_SCRIPT_TOPOLOGY_ROMAN_DISCONNECTED = 2,
    DWRITE_PANOSE_SCRIPT_TOPOLOGY_ROMAN_TRAILING = 3,
    DWRITE_PANOSE_SCRIPT_TOPOLOGY_ROMAN_CONNECTED = 4,
    DWRITE_PANOSE_SCRIPT_TOPOLOGY_CURSIVE_DISCONNECTED = 5,
    DWRITE_PANOSE_SCRIPT_TOPOLOGY_CURSIVE_TRAILING = 6,
    DWRITE_PANOSE_SCRIPT_TOPOLOGY_CURSIVE_CONNECTED = 7,
    DWRITE_PANOSE_SCRIPT_TOPOLOGY_BLACKLETTER_DISCONNECTED = 8,
    DWRITE_PANOSE_SCRIPT_TOPOLOGY_BLACKLETTER_TRAILING = 9,
    DWRITE_PANOSE_SCRIPT_TOPOLOGY_BLACKLETTER_CONNECTED = 10
    }

.enum DWRITE_PANOSE_SCRIPT_FORM {
    DWRITE_PANOSE_SCRIPT_FORM_ANY = 0,
    DWRITE_PANOSE_SCRIPT_FORM_NO_FIT = 1,
    DWRITE_PANOSE_SCRIPT_FORM_UPRIGHT_NO_WRAPPING = 2,
    DWRITE_PANOSE_SCRIPT_FORM_UPRIGHT_SOME_WRAPPING = 3,
    DWRITE_PANOSE_SCRIPT_FORM_UPRIGHT_MORE_WRAPPING = 4,
    DWRITE_PANOSE_SCRIPT_FORM_UPRIGHT_EXTREME_WRAPPING = 5,
    DWRITE_PANOSE_SCRIPT_FORM_OBLIQUE_NO_WRAPPING = 6,
    DWRITE_PANOSE_SCRIPT_FORM_OBLIQUE_SOME_WRAPPING = 7,
    DWRITE_PANOSE_SCRIPT_FORM_OBLIQUE_MORE_WRAPPING = 8,
    DWRITE_PANOSE_SCRIPT_FORM_OBLIQUE_EXTREME_WRAPPING = 9,
    DWRITE_PANOSE_SCRIPT_FORM_EXAGGERATED_NO_WRAPPING = 10,
    DWRITE_PANOSE_SCRIPT_FORM_EXAGGERATED_SOME_WRAPPING = 11,
    DWRITE_PANOSE_SCRIPT_FORM_EXAGGERATED_MORE_WRAPPING = 12,
    DWRITE_PANOSE_SCRIPT_FORM_EXAGGERATED_EXTREME_WRAPPING = 13
    }

.enum DWRITE_PANOSE_FINIALS {
    DWRITE_PANOSE_FINIALS_ANY = 0,
    DWRITE_PANOSE_FINIALS_NO_FIT = 1,
    DWRITE_PANOSE_FINIALS_NONE_NO_LOOPS = 2,
    DWRITE_PANOSE_FINIALS_NONE_CLOSED_LOOPS = 3,
    DWRITE_PANOSE_FINIALS_NONE_OPEN_LOOPS = 4,
    DWRITE_PANOSE_FINIALS_SHARP_NO_LOOPS = 5,
    DWRITE_PANOSE_FINIALS_SHARP_CLOSED_LOOPS = 6,
    DWRITE_PANOSE_FINIALS_SHARP_OPEN_LOOPS = 7,
    DWRITE_PANOSE_FINIALS_TAPERED_NO_LOOPS = 8,
    DWRITE_PANOSE_FINIALS_TAPERED_CLOSED_LOOPS = 9,
    DWRITE_PANOSE_FINIALS_TAPERED_OPEN_LOOPS = 10,
    DWRITE_PANOSE_FINIALS_ROUND_NO_LOOPS = 11,
    DWRITE_PANOSE_FINIALS_ROUND_CLOSED_LOOPS = 12,
    DWRITE_PANOSE_FINIALS_ROUND_OPEN_LOOPS = 13
    }

.enum DWRITE_PANOSE_XASCENT {
    DWRITE_PANOSE_XASCENT_ANY = 0,
    DWRITE_PANOSE_XASCENT_NO_FIT = 1,
    DWRITE_PANOSE_XASCENT_VERY_LOW = 2,
    DWRITE_PANOSE_XASCENT_LOW = 3,
    DWRITE_PANOSE_XASCENT_MEDIUM = 4,
    DWRITE_PANOSE_XASCENT_HIGH = 5,
    DWRITE_PANOSE_XASCENT_VERY_HIGH = 6
    }

.enum DWRITE_PANOSE_DECORATIVE_CLASS {
    DWRITE_PANOSE_DECORATIVE_CLASS_ANY = 0,
    DWRITE_PANOSE_DECORATIVE_CLASS_NO_FIT = 1,
    DWRITE_PANOSE_DECORATIVE_CLASS_DERIVATIVE = 2,
    DWRITE_PANOSE_DECORATIVE_CLASS_NONSTANDARD_TOPOLOGY = 3,
    DWRITE_PANOSE_DECORATIVE_CLASS_NONSTANDARD_ELEMENTS = 4,
    DWRITE_PANOSE_DECORATIVE_CLASS_NONSTANDARD_ASPECT = 5,
    DWRITE_PANOSE_DECORATIVE_CLASS_INITIALS = 6,
    DWRITE_PANOSE_DECORATIVE_CLASS_CARTOON = 7,
    DWRITE_PANOSE_DECORATIVE_CLASS_PICTURE_STEMS = 8,
    DWRITE_PANOSE_DECORATIVE_CLASS_ORNAMENTED = 9,
    DWRITE_PANOSE_DECORATIVE_CLASS_TEXT_AND_BACKGROUND = 10,
    DWRITE_PANOSE_DECORATIVE_CLASS_COLLAGE = 11,
    DWRITE_PANOSE_DECORATIVE_CLASS_MONTAGE = 12
    }

.enum DWRITE_PANOSE_ASPECT {
    DWRITE_PANOSE_ASPECT_ANY = 0,
    DWRITE_PANOSE_ASPECT_NO_FIT = 1,
    DWRITE_PANOSE_ASPECT_SUPER_CONDENSED = 2,
    DWRITE_PANOSE_ASPECT_VERY_CONDENSED = 3,
    DWRITE_PANOSE_ASPECT_CONDENSED = 4,
    DWRITE_PANOSE_ASPECT_NORMAL = 5,
    DWRITE_PANOSE_ASPECT_EXTENDED = 6,
    DWRITE_PANOSE_ASPECT_VERY_EXTENDED = 7,
    DWRITE_PANOSE_ASPECT_SUPER_EXTENDED = 8,
    DWRITE_PANOSE_ASPECT_MONOSPACED = 9
    }

.enum DWRITE_PANOSE_FILL {
    DWRITE_PANOSE_FILL_ANY = 0,
    DWRITE_PANOSE_FILL_NO_FIT = 1,
    DWRITE_PANOSE_FILL_STANDARD_SOLID_FILL = 2,
    DWRITE_PANOSE_FILL_NO_FILL = 3,
    DWRITE_PANOSE_FILL_PATTERNED_FILL = 4,
    DWRITE_PANOSE_FILL_COMPLEX_FILL = 5,
    DWRITE_PANOSE_FILL_SHAPED_FILL = 6,
    DWRITE_PANOSE_FILL_DRAWN_DISTRESSED = 7,
    }

.enum DWRITE_PANOSE_LINING {
    DWRITE_PANOSE_LINING_ANY = 0,
    DWRITE_PANOSE_LINING_NO_FIT = 1,
    DWRITE_PANOSE_LINING_NONE = 2,
    DWRITE_PANOSE_LINING_INLINE = 3,
    DWRITE_PANOSE_LINING_OUTLINE = 4,
    DWRITE_PANOSE_LINING_ENGRAVED = 5,
    DWRITE_PANOSE_LINING_SHADOW = 6,
    DWRITE_PANOSE_LINING_RELIEF = 7,
    DWRITE_PANOSE_LINING_BACKDROP = 8
    }

.enum DWRITE_PANOSE_DECORATIVE_TOPOLOGY {
    DWRITE_PANOSE_DECORATIVE_TOPOLOGY_ANY = 0,
    DWRITE_PANOSE_DECORATIVE_TOPOLOGY_NO_FIT = 1,
    DWRITE_PANOSE_DECORATIVE_TOPOLOGY_STANDARD = 2,
    DWRITE_PANOSE_DECORATIVE_TOPOLOGY_SQUARE = 3,
    DWRITE_PANOSE_DECORATIVE_TOPOLOGY_MULTIPLE_SEGMENT = 4,
    DWRITE_PANOSE_DECORATIVE_TOPOLOGY_ART_DECO = 5,
    DWRITE_PANOSE_DECORATIVE_TOPOLOGY_UNEVEN_WEIGHTING = 6,
    DWRITE_PANOSE_DECORATIVE_TOPOLOGY_DIVERSE_ARMS = 7,
    DWRITE_PANOSE_DECORATIVE_TOPOLOGY_DIVERSE_FORMS = 8,
    DWRITE_PANOSE_DECORATIVE_TOPOLOGY_LOMBARDIC_FORMS = 9,
    DWRITE_PANOSE_DECORATIVE_TOPOLOGY_UPPER_CASE_IN_LOWER_CASE = 10,
    DWRITE_PANOSE_DECORATIVE_TOPOLOGY_IMPLIED_TOPOLOGY = 11,
    DWRITE_PANOSE_DECORATIVE_TOPOLOGY_HORSESHOE_E_AND_A = 12,
    DWRITE_PANOSE_DECORATIVE_TOPOLOGY_CURSIVE = 13,
    DWRITE_PANOSE_DECORATIVE_TOPOLOGY_BLACKLETTER = 14,
    DWRITE_PANOSE_DECORATIVE_TOPOLOGY_SWASH_VARIANCE = 15
    }

.enum DWRITE_PANOSE_CHARACTER_RANGES {
    DWRITE_PANOSE_CHARACTER_RANGES_ANY = 0,
    DWRITE_PANOSE_CHARACTER_RANGES_NO_FIT = 1,
    DWRITE_PANOSE_CHARACTER_RANGES_EXTENDED_COLLECTION = 2,
    DWRITE_PANOSE_CHARACTER_RANGES_LITERALS = 3,
    DWRITE_PANOSE_CHARACTER_RANGES_NO_LOWER_CASE = 4,
    DWRITE_PANOSE_CHARACTER_RANGES_SMALL_CAPS = 5
    }

.enum DWRITE_PANOSE_SYMBOL_KIND {
    DWRITE_PANOSE_SYMBOL_KIND_ANY = 0,
    DWRITE_PANOSE_SYMBOL_KIND_NO_FIT = 1,
    DWRITE_PANOSE_SYMBOL_KIND_MONTAGES = 2,
    DWRITE_PANOSE_SYMBOL_KIND_PICTURES = 3,
    DWRITE_PANOSE_SYMBOL_KIND_SHAPES = 4,
    DWRITE_PANOSE_SYMBOL_KIND_SCIENTIFIC = 5,
    DWRITE_PANOSE_SYMBOL_KIND_MUSIC = 6,
    DWRITE_PANOSE_SYMBOL_KIND_EXPERT = 7,
    DWRITE_PANOSE_SYMBOL_KIND_PATTERNS = 8,
    DWRITE_PANOSE_SYMBOL_KIND_BOARDERS = 9,
    DWRITE_PANOSE_SYMBOL_KIND_ICONS = 10,
    DWRITE_PANOSE_SYMBOL_KIND_LOGOS = 11,
    DWRITE_PANOSE_SYMBOL_KIND_INDUSTRY_SPECIFIC = 12
    }

.enum DWRITE_PANOSE_SYMBOL_ASPECT_RATIO {
    DWRITE_PANOSE_SYMBOL_ASPECT_RATIO_ANY = 0,
    DWRITE_PANOSE_SYMBOL_ASPECT_RATIO_NO_FIT = 1,
    DWRITE_PANOSE_SYMBOL_ASPECT_RATIO_NO_WIDTH = 2,
    DWRITE_PANOSE_SYMBOL_ASPECT_RATIO_EXCEPTIONALLY_WIDE = 3,
    DWRITE_PANOSE_SYMBOL_ASPECT_RATIO_SUPER_WIDE = 4,
    DWRITE_PANOSE_SYMBOL_ASPECT_RATIO_VERY_WIDE = 5,
    DWRITE_PANOSE_SYMBOL_ASPECT_RATIO_WIDE = 6,
    DWRITE_PANOSE_SYMBOL_ASPECT_RATIO_NORMAL = 7,
    DWRITE_PANOSE_SYMBOL_ASPECT_RATIO_NARROW = 8,
    DWRITE_PANOSE_SYMBOL_ASPECT_RATIO_VERY_NARROW = 9
    }

.enum DWRITE_OUTLINE_THRESHOLD {
    DWRITE_OUTLINE_THRESHOLD_ANTIALIASED,
    DWRITE_OUTLINE_THRESHOLD_ALIASED
    }

.enum DWRITE_BASELINE {
    DWRITE_BASELINE_DEFAULT,
    DWRITE_BASELINE_ROMAN,
    DWRITE_BASELINE_CENTRAL,
    DWRITE_BASELINE_MATH,
    DWRITE_BASELINE_HANGING,
    DWRITE_BASELINE_IDEOGRAPHIC_BOTTOM,
    DWRITE_BASELINE_IDEOGRAPHIC_TOP,
    DWRITE_BASELINE_MINIMUM,
    DWRITE_BASELINE_MAXIMUM,
    }

.enum DWRITE_VERTICAL_GLYPH_ORIENTATION {
    DWRITE_VERTICAL_GLYPH_ORIENTATION_DEFAULT,
    DWRITE_VERTICAL_GLYPH_ORIENTATION_STACKED,
    }

.enum DWRITE_GLYPH_ORIENTATION_ANGLE {
    DWRITE_GLYPH_ORIENTATION_ANGLE_0_DEGREES,
    DWRITE_GLYPH_ORIENTATION_ANGLE_90_DEGREES,
    DWRITE_GLYPH_ORIENTATION_ANGLE_180_DEGREES,
    DWRITE_GLYPH_ORIENTATION_ANGLE_270_DEGREES,
    }


.template DWRITE_FONT_METRICS1 : public DWRITE_FONT_METRICS STRUC
glyphBoxLeft		INT16 ?
glyphBoxTop		INT16 ?
glyphBoxRight		INT16 ?
glyphBoxBottom		INT16 ?
subscriptPositionX	INT16 ?
subscriptPositionY	INT16 ?
subscriptSizeX		INT16 ?
subscriptSizeY		INT16 ?
superscriptPositionX	INT16 ?
superscriptPositionY	INT16 ?
superscriptSizeX	INT16 ?
superscriptSizeY	INT16 ?
hasTypographicMetrics	BOOL ?
.ENDS


DWRITE_CARET_METRICS	STRUC
slopeRise		INT16 ?
slopeRun		INT16 ?
offs			INT16 ?
DWRITE_CARET_METRICS	ENDS

DWRITE_PANOSE		UNION
values			UINT8 10 dup(?)
familyKind		UINT8 ?
STRUC text
familyKind		UINT8 ?
serifStyle		UINT8 ?
weight			UINT8 ?
proportion		UINT8 ?
contrast		UINT8 ?
strokeVariation		UINT8 ?
armStyle		UINT8 ?
letterform		UINT8 ?
midline			UINT8 ?
xHeight			UINT8 ?
ENDS
STRUC script
familyKind		UINT8 ?
toolKind		UINT8 ?
weight			UINT8 ?
spacing			UINT8 ?
aspectRatio		UINT8 ?
contrast		UINT8 ?
scriptTopology		UINT8 ?
scriptForm		UINT8 ?
finials			UINT8 ?
xAscent			UINT8 ?
ENDS
STRUC decorative
familyKind		UINT8 ?
decorativeClass		UINT8 ?
weight			UINT8 ?
aspect			UINT8 ?
contrast		UINT8 ?
serifVariant		UINT8 ?
fill			UINT8 ?
lining			UINT8 ?
decorativeTopology	UINT8 ?
characterRange		UINT8 ?
ENDS
STRUC symbol
familyKind		UINT8 ?
symbolKind		UINT8 ?
weight			UINT8 ?
spacing			UINT8 ?
aspectRatioAndContrast	UINT8 ?
aspectRatio94		UINT8 ?
aspectRatio119		UINT8 ?
aspectRatio157		UINT8 ?
aspectRatio163		UINT8 ?
aspectRatio211		UINT8 ?
ENDS
DWRITE_PANOSE		ENDS


DWRITE_UNICODE_RANGE	STRUC
first			UINT32 ?
last			UINT32 ?
DWRITE_UNICODE_RANGE	ENDS


DWRITE_SCRIPT_PROPERTIES STRUC
isoScriptCode		UINT32 ?
isoScriptNumber		UINT32 ?
clusterLookahead	UINT32 ?
justificationCharacter	UINT32 ?
flags			UINT32 ?
;restrictCaretToClusters      : 1 UINT32 ?
;usesWordDividers	      : 1 UINT32 ?
;isDiscreteWriting	      : 1 UINT32 ?
;isBlockWriting		      : 1 UINT32 ?
;isDistributedWithinCluster   : 1 UINT32 ?
;isConnectedWriting	      : 1 UINT32 ?
;isCursiveWriting	      : 1 UINT32 ?
;reserved		      : 25 UINT32 ?
DWRITE_SCRIPT_PROPERTIES ENDS


DWRITE_JUSTIFICATION_OPPORTUNITY STRUC
expansionMinimum	FLOAT ?
expansionMaximum	FLOAT ?
compressionMaximum	FLOAT ?
flags			UINT32 ?
;expansionPriority	   : 8 UINT32 ?
;compressionPriority	   : 8 UINT32 ?
;allowResidualExpansion	   : 1 UINT32 ?
;allowResidualCompression  : 1 UINT32 ?
;applyToLeadingEdge	   : 1 UINT32 ?
;applyToTrailingEdge	   : 1 UINT32 ?
;reserved		   : 12 UINT32 ?
DWRITE_JUSTIFICATION_OPPORTUNITY ENDS


DEFINE_IIDX(IDWriteFactory1, "30572f99-dac6-41db-a16e-0486307e606a")

.comdef IDWriteFactory1 : public IUnknown
; 0
    GetSystemFontCollection0	proc :ptr ptr IDWriteFontCollection, :BOOL
    CreateCustomFontCollection	proc :ptr IDWriteFontCollectionLoader, :UINT32, :ptr ptr IDWriteFontCollection
    RegisterFontCollectionLoader proc :ptr IDWriteFontCollectionLoader
    UnregisterFontCollectionLoader proc :ptr IDWriteFontCollectionLoader
    CreateFontFileReference	proc :ptr WCHAR, :ptr FILETIME, :ptr ptr IDWriteFontFile
    CreateCustomFontFileReference proc :UINT32, :ptr IDWriteFontFileLoader, :ptr ptr IDWriteFontFile
    CreateFontFace		proc :DWRITE_FONT_FACE_TYPE, :UINT32, :UINT32, :DWRITE_FONT_SIMULATIONS, :ptr ptr IDWriteFontFace
    CreateRenderingParams	proc :ptr ptr IDWriteRenderingParams
    CreateMonitorRenderingParams proc :HMONITOR, :ptr ptr IDWriteRenderingParams
    CreateCustomRenderingParams0 proc :FLOAT, :FLOAT, :FLOAT, :DWRITE_PIXEL_GEOMETRY, :DWRITE_RENDERING_MODE, :ptr ptr IDWriteRenderingParams
    RegisterFontFileLoader	proc :ptr IDWriteFontFileLoader
    UnregisterFontFileLoader	proc :ptr IDWriteFontFileLoader
    CreateTextFormat		proc :ptr ptr WCHAR, :ptr IDWriteFontCollection, :DWRITE_FONT_WEIGHT, :DWRITE_FONT_STYLE, :DWRITE_FONT_STRETCH, :FLOAT, :ptr WCHAR, :ptr ptr IDWriteTextFormat
    CreateTypography		proc :ptr ptr IDWriteTypography
    GetGdiInterop		proc :ptr ptr IDWriteGdiInterop
    CreateTextLayout		proc :ptr WCHAR, :UINT32, :ptr IDWriteTextFormat,  :FLOAT, :FLOAT, :ptr ptr IDWriteTextLayout
    CreateGdiCompatibleTextLayout proc :UINT32, :ptr IDWriteTextFormat, :FLOAT, :FLOAT, :FLOAT, :ptr DWRITE_MATRIX, :BOOL, :ptr ptr IDWriteTextLayout
    CreateEllipsisTrimmingSign	proc :ptr IDWriteTextFormat, :ptr ptr IDWriteInlineObject
    CreateTextAnalyzer		proc :ptr ptr IDWriteTextAnalyzer
    CreateNumberSubstitution	proc :DWRITE_NUMBER_SUBSTITUTION_METHOD, :ptr WCHAR, :BOOL, :ptr ptr IDWriteNumberSubstitution
    CreateGlyphRunAnalysis	proc :ptr DWRITE_GLYPH_RUN, :FLOAT, :ptr DWRITE_MATRIX, :DWRITE_RENDERING_MODE, :DWRITE_MEASURING_MODE, :FLOAT, :FLOAT, :ptr ptr IDWriteGlyphRunAnalysis
; 1
    GetEudcFontCollection	proc :ptr ptr IDWriteFontCollection, :BOOL
    CreateCustomRenderingParams proc :FLOAT, :FLOAT, :FLOAT, :FLOAT, :DWRITE_PIXEL_GEOMETRY, :DWRITE_RENDERING_MODE, :ptr ptr IDWriteRenderingParams1
    .ENDS

DEFINE_IIDX(IDWriteFontFace1, "a71efdb4-9fdb-4838-ad90-cfc3be8c3daf")

.comdef IDWriteFontFace1 : public IDWriteFontFace

    GetMetrics2		proc :ptr DWRITE_FONT_METRICS1
    GetGdiCompatibleMetrics2 proc :FLOAT, :FLOAT, :ptr DWRITE_MATRIX, :ptr DWRITE_FONT_METRICS1
    GetCaretMetrics	proc :ptr DWRITE_CARET_METRICS
    GetUnicodeRanges	proc :UINT32, :ptr DWRITE_UNICODE_RANGE, :ptr UINT32
    IsMonospacedFont	proc
    GetDesignGlyphAdvances proc :UINT32, :ptr UINT16, :ptr INT32, :BOOL
    GetGdiCompatibleGlyphAdvances proc :FLOAT, :FLOAT, :ptr DWRITE_MATRIX, :BOOL, :BOOL, :UINT32, :ptr INT32
    GetKerningPairAdjustments proc :UINT32, :ptr INT32
    HasKerningPairs	proc
    GetRecommendedRenderingMode2 proc :FLOAT, :FLOAT, :FLOAT, :ptr DWRITE_MATRIX, :BOOL, :DWRITE_OUTLINE_THRESHOLD, :DWRITE_MEASURING_MODE, :ptr DWRITE_RENDERING_MODE
    GetVerticalGlyphVariants proc :UINT32, :ptr UINT16
    HasVerticalGlyphVariants proc
    .ENDS


DEFINE_IIDX(IDWriteFont1, "acd16696-8c14-4f5d-877e-fe3fc1d32738")

.comdef IDWriteFont1 : public IDWriteFont

    GetMetrics2		proc :ptr DWRITE_FONT_METRICS1
    GetPanose		proc :ptr DWRITE_PANOSE
    GetUnicodeRanges	proc :UINT32, :ptr DWRITE_UNICODE_RANGE, :ptr UINT32
    IsMonospacedFont	proc
    .ENDS

DEFINE_IIDX(IDWriteRenderingParams1, "94413cf4-a6fc-4248-8b50-6674348fcad3")

.comdef IDWriteRenderingParams1 : public IDWriteRenderingParams

     GetGrayscaleEnhancedContrast proc
     .ENDS


DEFINE_IIDX(IDWriteTextAnalyzer1, "80DAD800-E21F-4E83-96CE-BFCCE500DB7C")

.comdef IDWriteTextAnalyzer1 : public IDWriteTextAnalyzer

    ApplyCharacterSpacing	proc :FLOAT, :FLOAT, :FLOAT, :UINT32, :UINT32, :ptr UINT16, :ptr FLOAT, :ptr DWRITE_GLYPH_OFFSET, :ptr DWRITE_SHAPING_GLYPH_PROPERTIES, :ptr FLOAT, :ptr DWRITE_GLYPH_OFFSET
    GetBaseline			proc :ptr IDWriteFontFace, :DWRITE_BASELINE, :BOOL, :BOOL, :DWRITE_SCRIPT_ANALYSIS, :ptr WCHAR, :ptr INT32, :ptr BOOL
    AnalyzeVerticalGlyphOrientation proc :ptr IDWriteTextAnalysisSource1, :UINT32, :UINT32, :ptr IDWriteTextAnalysisSink1
    GetGlyphOrientationTransform proc :DWRITE_GLYPH_ORIENTATION_ANGLE, :BOOL, :ptr DWRITE_MATRIX
    GetScriptProperties		proc :DWRITE_SCRIPT_ANALYSIS, :ptr DWRITE_SCRIPT_PROPERTIES
    GetTextComplexity		proc :UINT32, :ptr IDWriteFontFace, :ptr BOOL, :ptr UINT32, :ptr UINT16
    GetJustificationOpportunities proc :ptr IDWriteFontFace, :FLOAT, :DWRITE_SCRIPT_ANALYSIS, :UINT32, :UINT32, :ptr UINT16, :ptr DWRITE_SHAPING_GLYPH_PROPERTIES, :ptr DWRITE_JUSTIFICATION_OPPORTUNITY
    JustifyGlyphAdvances	proc :FLOAT, :UINT32, :ptr FLOAT, :ptr DWRITE_GLYPH_OFFSET, :ptr FLOAT, :ptr DWRITE_GLYPH_OFFSET
    GetJustifiedGlyphs		proc :ptr IDWriteFontFace, :FLOAT, :DWRITE_SCRIPT_ANALYSIS, :UINT32, :UINT32, :UINT32, :ptr UINT16, :ptr FLOAT, :ptr FLOAT, :ptr DWRITE_GLYPH_OFFSET, :ptr DWRITE_SHAPING_GLYPH_PROPERTIES, :ptr UINT32, :ptr UINT16, :ptr UINT16, :ptr FLOAT, :ptr DWRITE_GLYPH_OFFSET
    .ENDS


DEFINE_IIDX(IDWriteTextAnalysisSource1, "639CFAD8-0FB4-4B21-A58A-067920120009")

.comdef IDWriteTextAnalysisSource1 : public IDWriteTextAnalysisSource

    GetVerticalGlyphOrientation proc :UINT32, :ptr UINT32, :ptr DWRITE_VERTICAL_GLYPH_ORIENTATION, :ptr UINT8
    .ENDS


DEFINE_IIDX(IDWriteTextAnalysisSink1, "B0D941A0-85E7-4D8B-9FD3-5CED9934482A")

.comdef IDWriteTextAnalysisSink1 : public IDWriteTextAnalysisSink

    SetGlyphOrientation proc :UINT32, :UINT32, :DWRITE_GLYPH_ORIENTATION_ANGLE, :UINT8, :BOOL, :BOOL
    .ENDS


DEFINE_IIDX(IDWriteTextLayout1, "9064D822-80A7-465C-A986-DF65F78B8FEB")

.comdef IDWriteTextLayout1 : public IDWriteTextLayout

    SetPairKerning proc :BOOL, :DWRITE_TEXT_RANGE
    GetPairKerning proc :UINT32, :ptr isPairKerningEnabled, :ptr DWRITE_TEXT_RANGE
    SetCharacterSpacing proc :FLOAT, :FLOAT, :FLOAT, :DWRITE_TEXT_RANGE
    GetCharacterSpacing proc :UINT32, :ptr FLOAT, :ptr FLOAT, :ptr FLOAT, :ptr DWRITE_TEXT_RANGE
    .ENDS

.enum DWRITE_TEXT_ANTIALIAS_MODE {
    DWRITE_TEXT_ANTIALIAS_MODE_CLEARTYPE,
    DWRITE_TEXT_ANTIALIAS_MODE_GRAYSCALE
    }


DEFINE_IIDX(IDWriteBitmapRenderTarget1, "791e8298-3ef3-4230-9880-c9bdecc42064")

.comdef IDWriteBitmapRenderTarget1 : public IDWriteBitmapRenderTarget

    GetTextAntialiasMode proc
    SetTextAntialiasMode proc :DWRITE_TEXT_ANTIALIAS_MODE
    .ENDS

endif