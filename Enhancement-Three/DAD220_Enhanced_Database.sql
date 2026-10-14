-- DAD 220 Database Project
-- Enhanced Version
-- Jack Markham
-- CS 499 ePortfolio

CREATE DATABASE IF NOT EXISTS QuantigrationUpdates;

USE QuantigrationUpdates;

SHOW DATABASES;

-- Create the Customers table
CREATE TABLE Customers (
    CustomerID INT AUTO_INCREMENT,
    FirstName VARCHAR(50),
    LastName VARCHAR(50),
    StreetAddress VARCHAR(255),
    City VARCHAR(100),
    State VARCHAR(50),
    ZipCode VARCHAR(10),
    Telephone VARCHAR(20),
    PRIMARY KEY (CustomerID)
);

-- Create the Orders table
CREATE TABLE Orders (
    OrderID INT AUTO_INCREMENT,
    CustomerID INT,
    SKU VARCHAR(50),
    ProductDescription VARCHAR(255),
    ProductType VARCHAR(100),
    PortCount INT,
    OrderDate DATE,
    OrderAmount DECIMAL(10,2),
    PRIMARY KEY (OrderID),
    FOREIGN KEY (CustomerID) REFERENCES Customers(CustomerID)
);

-- Create the RMA table
CREATE TABLE RMA (
    RMAID INT AUTO_INCREMENT,
    OrderID INT,
    RMAReason VARCHAR(255),
    RMADate DATE,
    RMAStatus VARCHAR(50),
    PRIMARY KEY (RMAID),
    FOREIGN KEY (OrderID) REFERENCES Orders(OrderID)
);

SHOW TABLES;

-- Reconstructed sample data used for testing the enhanced database

INSERT INTO Customers
(CustomerID, FirstName, LastName, StreetAddress, City, State, ZipCode, Telephone)
VALUES
(100004, 'Luke', 'Skywalker', '15 Maiden Lane', 'New York', 'NY', '10022', '212-555-1234'),
(100005, 'Winston', 'Smith', '123 Sycamore Street', 'Greensboro', 'NC', '27401', '919-555-6623'),
(100006, 'MaryAnne', 'Jenkins', '1 Coconut Way', 'Jupiter', 'FL', '33458', '321-555-8907'),
(100007, 'Janet', 'Williams', '55 Redondo Beach Blvd', 'Torrence', 'CA', '90501', '310-555-5678');

INSERT INTO Orders
(OrderID, CustomerID, SKU, ProductDescription, ProductType, PortCount, OrderDate, OrderAmount)
VALUES
(1204305, 100004, 'ADV-24-10C', 'Advanced Switch 10GigE Copper 24 Port', 'Product A', 24, '2026-01-10', 1200.00),
(1204306, 100005, 'ADV-48-10F', 'Advanced Switch 10GigE Copper/Fiber 44 Port', 'Product A', 44, '2026-01-12', 1800.00),
(1204307, 100006, 'ENT-24-10F', 'Enterprise Switch 10GigE SFP+ 24 Port', 'Product B', 24, '2026-01-15', 2200.00),
(1204308, 100007, 'ENT-48-10F', 'Enterprise Switch 10GigE SFP+ 48 Port', 'Product C', 48, '2026-01-18', 3200.00);

INSERT INTO RMA
(RMAID, OrderID, RMAReason, RMADate, RMAStatus)
VALUES
(1, 1204305, 'Defective product', '2026-02-01', 'Complete'),
(2, 1204306, 'Incorrect item', '2026-02-03', 'Complete'),
(3, 1204307, 'Defective product', '2026-02-05', 'Pending'),
(4, 1204308, 'Customer return', '2026-02-07', 'Complete');

-- Verify the sample data
SELECT * FROM Customers;
SELECT * FROM Orders;
SELECT * FROM RMA;

-- =====================================================
-- Enhanced Queries
-- These queries provide more useful information by
-- combining and summarizing data from multiple tables.
-- =====================================================

-- Display customers with their orders
SELECT
    c.CustomerID,
    c.FirstName,
    c.LastName,
    o.OrderID,
    o.ProductDescription,
    o.OrderAmount
FROM Customers c
JOIN Orders o ON c.CustomerID = o.CustomerID
ORDER BY c.CustomerID;

-- Show total amount spent by each customer
SELECT
    c.CustomerID,
    c.FirstName,
    c.LastName,
    COUNT(o.OrderID) AS TotalOrders,
    SUM(o.OrderAmount) AS TotalSpent
FROM Customers c
JOIN Orders o ON c.CustomerID = o.CustomerID
GROUP BY c.CustomerID, c.FirstName, c.LastName
ORDER BY TotalSpent DESC;

-- Display RMA information with customer and order details
SELECT
    r.RMAID,
    c.FirstName,
    c.LastName,
    o.OrderID,
    o.ProductDescription,
    r.RMAReason,
    r.RMADate,
    r.RMAStatus
FROM RMA r
JOIN Orders o ON r.OrderID = o.OrderID
JOIN Customers c ON o.CustomerID = c.CustomerID
ORDER BY r.RMADate;