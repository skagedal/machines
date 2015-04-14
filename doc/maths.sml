(* SML code to accompany "maths.txt"
 * Simon Kågedal 1998-11-28
 *)

load "Math";
open Math;

(* MIDI pitchbend procedure 
 *
 * Type: real * real * int -> real
 *
 * `base' is the base frequency of the note we want to bend
 * `max' decides how sensitive the pitchbend is; a value 12.0 gives
 *	a max pitch bend of an octave
 * `wheel' is the MIDI pitch bend value
 *)

local
    fun freq (base, n) = pow (2.0, ln (base)/ln(2.0) + (n/12.0));
    fun midipitchbend (x) = real (x - 0x2000) / real (0x2000);
in
    fun pitchbend (base, max, wheel) =
	freq (base, max * midipitchbend (wheel))
end;
