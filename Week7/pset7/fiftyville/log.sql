-- Keep a log of any SQL queries you execute as you solve the mystery.

-- Read the description of the crime scene.
SELECT description FROM crime_scene_reports WHERE year = "2021" AND month = "7" AND day = "28" AND street = "Humphrey Street";

-- Check the transcript from that day that mentioned bakery.
SELECT transcript FROM interviews WHERE year = "2021" AND month = "7" AND day = "28" AND transcript LIKE "%bakery%";

-- Check activity and license plate from a 10 minutes rangin from the time of the theft.
SELECT activity, license_plate FROM bakery_security_logs WHERE year = "2021" AND month = "7" AND day = "28" AND hour = "10" AND minute BETWEEN 15 AND 25;


-- Intersect the queries possible from the transcript to get the name of the thief.

-- Check the names of the people that have that license plates who left the bakery at that timeframe.
SELECT name FROM people
JOIN bakery_security_logs
ON people.license_plate = bakery_security_logs.license_plate
WHERE year = "2021" AND month = "7" AND day = "28" AND hour = "10" AND minute BETWEEN 15 AND 25 AND activity = "exit"
INTERSECT
-- Check the names of people who withdraw at the atm on Leggett Street that day.
SELECT DISTINCT name FROM people
JOIN bank_accounts
ON people.id = bank_accounts.person_id
JOIN atm_transactions
ON bank_accounts.account_number = atm_transactions.account_number
WHERE year = "2021" AND month = "7" AND day = "28" AND atm_location = "Leggett Street" AND transaction_type = "withdraw"
INTERSECT
-- Check who made a call of less than 1 minute from the day of the theft.
SELECT name from people
JOIN phone_calls
ON people.phone_number = phone_calls.caller
WHERE year = "2021" AND month = "7" AND day = "28" AND duration < 60
INTERSECT
-- Check who took a flight on the day after the theft.
SELECT name FROM people
JOIN passengers
ON people.passport_number = passengers.passport_number
WHERE flight_id = (SELECT id FROM flights WHERE year = "2021" AND month = "7" AND day = "29" ORDER BY hour, minute LIMIT 1);

-- Get the destiny from the first flight of the day after the robbery.
SELECT city FROM airports
WHERE id = (SELECT destination_airport_id FROM flights WHERE year = "2021" AND month = "7" AND day = "29" ORDER BY hour, minute LIMIT 1);

-- Get the accomplice by the phone reccord.
SELECT name FROM people
JOIN phone_calls
ON people.phone_number = phone_calls.receiver
WHERE year = "2021" AND month = "7" AND day = "28" AND duration < 60 AND caller = (SELECT phone_number FROM people WHERE name = "Bruce");
