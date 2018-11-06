char number_to_char(int number)
{
  if (number < 10)
    return ((char) number + ASCII_NUMBER_DELTA);
  else
    return ((char) number + ASCII_LETTER_DELTA);
}
