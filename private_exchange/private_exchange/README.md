1. Describe how your exchange works.
The program first sets up a signal handler which will store the PID of each incoming SIGUSR1 in a signal array, indicating incoming
message and its origin.
Each message undergoes safety checks 
and is sliced into an argument array and the exchange calls their designated functions. 
For an Order Request, a new Order is instantiated and appended on to the orderbook, and an ACCEPTED message is sent to the trader. 
If applicable, the Order is placed against other Orders in the orderbook to match a Trade with the best buy/sell order available based on price-time priority. A signal is sent to the buyer and seller in the trade along with a FILL message. If the exchange receives an amend request, it will update the order with updated price and quantity, along with the latest order index, therefore it loses it's time priority. The exchange disconnects when all traders disconnect.


2. Describe your design decisions for the trader and how it's fault-tolerant.

The design is kept simple to avoid long runtimes and losing incoming signal. Each incoming signal increments a signal count, which indicates a missed incoming message while the program is processing other operations. Each message is placed through safety checks. There are three states for each message: Order received, valid and invalid messages. If there is an incoming sell order and passes safety check, the trader will reply with a signal and buy order message. If the order quantity is > 1000, the trader will disconnect. If the message is invalid, the order ID increments to avoid reuse of order id.

3. Describe your tests and how to run them.
