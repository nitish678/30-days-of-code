#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/**
 * @file
 * @brief Simple blockchain data structure implementation
 * @author PARTH
 * @date 2026-02-10
 *
 * Demonstrates basic blockchain concepts:
 * - Linked blocks
 * - Hash verification
 * - Simple transaction structure
 */

#define MAX_TRANSACTIONS 10
#define HASH_LENGTH 32

/**
 * @brief Simple transaction structure
 */
typedef struct {
    char sender[64];
    char receiver[64];
    double amount;
    time_t timestamp;
} Transaction;

/**
 * @brief Block structure containing transactions
 */
typedef struct Block {
    int index;
    char previous_hash[HASH_LENGTH + 1];
    Transaction transactions[MAX_TRANSACTIONS];
    int transaction_count;
    time_t timestamp;
    char current_hash[HASH_LENGTH + 1];
    struct Block* next;
} Block;

/**
 * @brief Calculate simple hash (simulated)
 * @param data Data to hash
 * @return Hash value (unsigned long)
 */
unsigned long simple_hash(const char* data) {
    unsigned long hash = 5381;
    int c;
    while ((c = *data++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

/**
 * @brief Add transaction to block
 * @param block Pointer to block
 * @param sender Transaction sender
 * @param receiver Transaction receiver
 * @param amount Transaction amount
 */
void add_transaction(Block* block, const char* sender, const char* receiver, double amount) {
    if (block->transaction_count < MAX_TRANSACTIONS) {
        Transaction* tx = &block->transactions[block->transaction_count];
        snprintf(tx->sender, 64, "%s", sender);
        snprintf(tx->receiver, 64, "%s", receiver);
        tx->amount = amount;
        tx->timestamp = time(NULL);
        block->transaction_count++;
        printf("Transaction added: %s -> %s (%.2f BTC)\n", sender, receiver, amount);
    } else {
        printf("Block full! Cannot add transaction.\n");
    }
}

/**
 * @brief Calculate block hash
 * @param block Block to hash
 */
void calculate_block_hash(Block* block) {
    char hash_data[256];
    snprintf(hash_data, sizeof(hash_data), "%d%s%ld",
             block->index,
             block->previous_hash,
             block->timestamp);

    snprintf(block->current_hash, HASH_LENGTH, "%lx", simple_hash(hash_data));
}

/**
 * @brief Print block details
 * @param block Block to print
 */
void print_block(const Block* block) {
    printf("\n=== BLOCK #%d ===\n", block->index);
    printf("Previous Hash: %s\n", block->previous_hash);
    printf("Current Hash: %s\n", block->current_hash);
    printf("Timestamp: %ld\n", (long)block->timestamp);
    printf("Transactions: %d\n", block->transaction_count);

    for (int i = 0; i < block->transaction_count; i++) {
        printf("  %s -> %s (%.2f BTC)\n",
               block->transactions[i].sender,
               block->transactions[i].receiver,
               block->transactions[i].amount);
    }
    printf("===================\n");
}

/**
 * @brief Create genesis block
 * @param blockchain Pointer to blockchain head
 */
void create_genesis_block(Block** blockchain) {
    *blockchain = (Block*)malloc(sizeof(Block));
    (*blockchain)->index = 0;
    strcpy((*blockchain)->previous_hash, "00000000000000000000000000000000000000");
    (*blockchain)->transaction_count = 0;
    (*blockchain)->timestamp = time(NULL);
    (*blockchain)->next = NULL;
    calculate_block_hash(*blockchain);
    printf("Genesis block created.\n");
}

/**
 * @brief Add new block to blockchain
 * @param blockchain Pointer to blockchain head
 */
void add_block(Block** blockchain) {
    // Find the last block
    Block* last_block = *blockchain;
    while (last_block->next != NULL) {
        last_block = last_block->next;
    }

    // Create new block
    Block* new_block = (Block*)malloc(sizeof(Block));
    new_block->index = last_block->index + 1;
    strcpy(new_block->previous_hash, last_block->current_hash);
    new_block->transaction_count = 0;
    new_block->timestamp = time(NULL);
    new_block->next = NULL;
    last_block->next = new_block;

    // Add a sample transaction
    add_transaction(new_block, "Alice", "Bob", 1.5);
    add_transaction(new_block, "Charlie", "Dave", 0.75);

    // Calculate hash
    calculate_block_hash(new_block);

    printf("Block #%d added to blockchain.\n", new_block->index);
}

/**
 * @brief Verify blockchain integrity
 * @param blockchain Pointer to blockchain head
 * @return 1 if valid, 0 if invalid
 */
int verify_blockchain(Block* blockchain) {
    Block* current = blockchain;
    while (current != NULL) {
        Block* next = current->next;
        if (next != NULL) {
            // Verify previous hash matches
            if (strcmp(next->previous_hash, current->current_hash) != 0) {
                printf("WARNING: Block #%d hash mismatch!\n", next->index);
                return 0;
            }
        }
        current = next;
    }
    return 1;
}

/**
 * @brief Print entire blockchain
 * @param blockchain Pointer to blockchain head
 */
void print_blockchain(Block* blockchain) {
    Block* current = blockchain;
    int count = 0;
    while (current != NULL) {
        print_block(current);
        current = current->next;
        count++;
    }
    printf("\nTotal blocks in blockchain: %d\n", count);
}

/**
 * @brief Free blockchain memory
 * @param blockchain Pointer to blockchain head
 */
void free_blockchain(Block* blockchain) {
    Block* current = blockchain;
    while (current != NULL) {
        Block* next = current->next;
        free(current);
        current = next;
    }
}

/**
 * @brief Main function
 */
int main() {
    printf("=== Simple Blockchain Implementation ===\n\n");
    printf("Author: PARTH\n");
    printf("Date: 2026-02-10\n\n");

    Block* blockchain = NULL;

    // Create genesis block
    create_genesis_block(&blockchain);

    // Add some blocks with transactions
    add_block(&blockchain);
    add_block(&blockchain);
    add_block(&blockchain);

    // Print blockchain
    print_blockchain(blockchain);

    // Verify integrity
    printf("\nVerifying blockchain integrity...\n");
    if (verify_blockchain(&blockchain)) {
        printf("Blockchain is valid!\n");
    } else {
        printf("Blockchain verification failed!\n");
    }

    // Free memory
    free_blockchain(blockchain);

    printf("\n=== Implementation Complete ===\n");

    return 0;
}
