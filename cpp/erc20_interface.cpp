/**
 * @file
 * @brief Smart contract interface for ERC-20 tokens
 * @author PARTH
 * @date 2026-02-10
 *
 * Demonstrates:
 * - ERC-20 standard interface
 * - Token transfer logic
 * - Approval mechanism
 * - Balance management
 */

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <cstdint>
#include <iomanip>

/**
 * @brief ERC-20 Token Interface
 */
class IERC20 {
public:
    virtual std::string name() const = 0;
    virtual std::string symbol() const = 0;
    virtual uint8_t decimals() const = 0;
    virtual uint256_t totalSupply() const = 0;
    virtual uint256_t balanceOf(address owner) const = 0;
    virtual uint256_t allowance(address owner, address spender) const = 0;
    virtual bool approve(address spender, uint256_t amount) = 0;
    virtual bool transfer(address to, uint256_t amount) = 0;
    virtual bool transferFrom(address from, address to, uint256_t amount) = 0;
    virtual ~IERC20() {}
};

/**
 * @brief Simple address type (simulated)
 */
using address = std::string;

/**
 * @brief Token contract implementation
 */
class TokenContract : public IERC20 {
private:
    std::string _name;
    std::string _symbol;
    uint8_t _decimals;
    uint256_t _totalSupply;
    std::map<address, uint256_t> _balances;
    std::map<address, std::map<address, uint256_t>> _allowances;

public:
    /**
     * @brief Constructor
     */
    TokenContract(
        std::string name,
        std::string symbol,
        uint8_t decimals,
        uint256_t initialSupply
    ) {
        _name = name;
        _symbol = symbol;
        _decimals = decimals;
        _totalSupply = initialSupply;

        // Mint initial supply to contract owner (address "0x0")
        _balances["0x0"] = initialSupply;

        std::cout << "Token created: " << name << " (" << symbol << ")" << std::endl;
        std::cout << "Initial supply: " << initialSupply << std::endl;
    }

    /**
     * @brief Get token name
     */
    std::string name() const override {
        return _name;
    }

    /**
     * @brief Get token symbol
     */
    std::string symbol() const override {
        return _symbol;
    }

    /**
     * @brief Get token decimals
     */
    uint8_t decimals() const override {
        return _decimals;
    }

    /**
     * @brief Get total supply
     */
    uint256_t totalSupply() const override {
        return _totalSupply;
    }

    /**
     * @brief Get balance of address
     */
    uint256_t balanceOf(address owner) const override {
        auto it = _balances.find(owner);
        if (it != _balances.end()) {
            return it->second;
        }
        return 0;
    }

    /**
     * @brief Get allowance
     */
    uint256_t allowance(address owner, address spender) const override {
        auto ownerIt = _allowances.find(owner);
        if (ownerIt != _allowances.end()) {
            auto spenderIt = ownerIt->second.find(spender);
            if (spenderIt != ownerIt->second.end()) {
                return spenderIt->second;
            }
        }
        return 0;
    }

    /**
     * @brief Approve spender to spend tokens
     */
    bool approve(address spender, uint256_t amount) override {
        if (balanceOf(msgSender()) < amount) {
            std::cout << "Approval failed: Insufficient balance" << std::endl;
            return false;
        }

        _allowances[msgSender()][spender] = amount;
        std::cout << "Approved " << amount << " for " << spender << std::endl;
        return true;
    }

    /**
     * @brief Transfer tokens
     */
    bool transfer(address to, uint256_t amount) override {
        address sender = msgSender();

        // Check balance
        if (balanceOf(sender) < amount) {
            std::cout << "Transfer failed: Insufficient balance" << std::endl;
            return false;
        }

        // Update balances
        _balances[sender] = balanceOf(sender) - amount;
        _balances[to] = balanceOf(to) + amount;

        std::cout << "Transfered " << amount << " from " << sender << " to " << to << std::endl;
        return true;
    }

    /**
     * @brief Transfer from approved allowance
     */
    bool transferFrom(address from, address to, uint256_t amount) override {
        address spender = msgSender();

        // Check allowance
        if (allowance(from, spender) < amount) {
            std::cout << "Transfer failed: Insufficient allowance" << std::endl;
            return false;
        }

        // Check balance
        if (balanceOf(from) < amount) {
            std::cout << "Transfer failed: Insufficient balance" << std::endl;
            return false;
        }

        // Update allowance
        _allowances[from][spender] = allowance(from, spender) - amount;

        // Update balances
        _balances[from] = balanceOf(from) - amount;
        _balances[to] = balanceOf(to) + amount;

        std::cout << "Transferred " << amount << " from " << from << " to " << to << std::endl;
        return true;
    }

    /**
     * @brief Get message sender (simulated)
     */
    address msgSender() const {
        return "0x0"; // In real contract, this is msg.sender
    }

    /**
     * @brief Print contract state
     */
    void printState() const {
        std::cout << "\n=== TOKEN CONTRACT STATE ===" << std::endl;
        std::cout << "Name: " << _name << std::endl;
        std::cout << "Symbol: " << _symbol << std::endl;
        std::cout << "Decimals: " << (int)_decimals << std::endl;
        std::cout << "Total Supply: " << _totalSupply << std::endl;
        std::cout << "\nBalances:" << std::endl;

        for (const auto& entry : _balances) {
            std::cout << "  " << entry.first << ": " << entry.second << std::endl;
        }
        std::cout << "\nTotal holders: " << _balances.size() << std::endl;
        std::cout << "=========================" << std::endl;
    }
};

/**
 * @brief Main demonstration function
 */
int main() {
    std::cout << "=".repeat(50) << std::endl;
    std::cout << "ERC-20 SMART CONTRACT INTERFACE" << std::endl;
    std::cout << "=".repeat(50) << std::endl;
    std::cout << "Author: PARTH" << std::endl;
    std::cout << "Date: 2026-02-10" << std::endl;
    std::cout << "=".repeat(50) << std::endl;
    std::cout << std::endl;

    // Create token instance
    TokenContract token("DemoToken", "DTK", 18, 1000000);

    // Print initial state
    token.printState();

    std::cout << "\n--- DEMONSTRATION ---" << std::endl;

    // Simulate transactions
    address alice = "0xAlice";
    address bob = "0xBob";
    address charlie = "0xCharlie";

    std::cout << "\n[Transaction 1] Alice approves Bob to spend 500" << std::endl;
    token.approve(bob, 500);

    std::cout << "\n[Transaction 2] Bob transfers 500 to Charlie" << std::endl;
    token.transferFrom(alice, charlie, 500);

    std::cout << "\n[Transaction 3] Alice transfers 200 to Bob" << std::endl;
    token.transfer(bob, 200);

    std::cout << "\n[Transaction 4] Bob transfers 100 to Charlie" << std::endl;
    token.transfer(bob, 100);

    // Print final state
    std::cout << "\n--- FINAL STATE ---" << std::endl;
    token.printState();

    // Display statistics
    std::cout << "\n=== TRANSACTION SUMMARY ===" << std::endl;
    std::cout << "Total Supply: " << token.totalSupply() << std::endl;
    std::cout << "Alice Balance: " << token.balanceOf(alice) << std::endl;
    std::cout << "Bob Balance: " << token.balanceOf(bob) << std::endl;
    std::cout << "Charlie Balance: " << token.balanceOf(charlie) << std::endl;

    // Calculate total value in circulation
    uint256_t circulating = 0;
    for (const auto& entry : *reinterpret_cast<std::map<address, uint256_t>*>(
        const_cast<TokenContract*>(&token)->_balances)) {
        circulating += entry.second;
    }

    std::cout << "Total in circulation: " << circulating << std::endl;
    std::cout << "Held by contract: " << (token.totalSupply() - circulating) << std::endl;
    std::cout << "=========================" << std::endl;

    return 0;
}
