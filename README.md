# Monthly-Budget-Tracker

## Purpose
To simplify keeping track of how much you spend and earn in a month, giving you an instant net view of your monthly budget.

## How does it work?
* The application can read your receipts by using a webcam and pointing it to a receipt. Once the receipt is detected you can
press the 'q' key and and the image of the receipt will be processed and transformed using the OpenCV library and read using the Tessereact OCR API. Once the total is extracted, it will be deducted from your budget for the month.
