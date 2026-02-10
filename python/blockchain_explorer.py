"""
@file
@brief Simple blockchain explorer - fetches and displays block data
@author PARTH
@date 2026-02-10
"""

import json
import time
from datetime import datetime
from typing import Dict, List, Optional

class Block:
    """Represents a blockchain block"""

    def __init__(self, index: int, previous_hash: str, timestamp: float,
                 transactions: List[Dict], hash: str):
        self.index = index
        self.previous_hash = previous_hash
        self.timestamp = timestamp
        self.transactions = transactions
        self.hash = hash

    def to_dict(self) -> Dict:
        return {
            'index': self.index,
            'previous_hash': self.previous_hash,
            'timestamp': self.timestamp,
            'transactions': self.transactions,
            'hash': self.hash,
            'transaction_count': len(self.transactions)
        }


class BlockchainExplorer:
    """Simple blockchain data explorer"""

    def __init__(self):
        self.blocks = []
        self.block_count = 0

    def create_genesis_block(self) -> Block:
        """Create the first (genesis) block"""
        genesis_block = Block(
            index=0,
            previous_hash="0" * 64,
            timestamp=time.time(),
            transactions=[{
                'sender': 'COINBASE',
                'receiver': 'MINER_REWARD',
                'amount': 50.0
            }],
            hash=self._calculate_block_hash(0, "0" * 64, [], 50.0)
        )
        self.blocks.append(genesis_block)
        self.block_count += 1
        print(f"Genesis block created at index {genesis_block.index}")
        return genesis_block

    def _calculate_block_hash(self, index: int, previous_hash: str,
                          timestamp: float, transactions: List[Dict]) -> str:
        """Calculate simple block hash"""
        # Simulate hash calculation (in real blockchain, this would use SHA-256)
        data = f"{index}{previous_hash}{timestamp}{len(transactions)}"
        # Use a simple hash representation (in production, use hashlib.sha256)
        hash_val = str(abs(hash(data) % (10 ** 12)))
        return hash_val.zfill(64)

    def add_block(self, transactions: List[Dict]) -> Block:
        """Add a new block with transactions"""
        previous_block = self.blocks[-1] if self.blocks else None

        if previous_block:
            new_index = previous_block.index + 1
        else:
            new_index = 0

        timestamp = time.time()
        block_hash = self._calculate_block_hash(new_index, previous_block.hash if previous_block else "0" * 64, timestamp, transactions)

        new_block = Block(
            index=new_index,
            previous_hash=previous_block.hash if previous_block else "0" * 64,
            timestamp=timestamp,
            transactions=transactions,
            hash=block_hash
        )

        self.blocks.append(new_block)
        self.block_count += 1

        print(f"Block #{new_block} added with {len(transactions)} transactions")
        return new_block

    def get_block_by_index(self, index: int) -> Optional[Block]:
        """Get block by index"""
        for block in self.blocks:
            if block.index == index:
                return block
        return None

    def get_latest_blocks(self, count: int = 5) -> List[Block]:
        """Get the latest N blocks"""
        return self.blocks[-count:]

    def get_blockchain_stats(self) -> Dict:
        """Get blockchain statistics"""
        if not self.blocks:
            return {
                'total_blocks': 0,
                'total_transactions': 0,
                'last_block_hash': None
            }

        total_transactions = sum(block.transaction_count for block in self.blocks)
        last_block = self.blocks[-1]

        return {
            'total_blocks': self.block_count,
            'total_transactions': total_transactions,
            'last_block_hash': last_block.hash,
            'last_block_index': last_block.index,
            'blockchain_age_seconds': time.time() - last_block.timestamp
        }

    def display_block(self, block: Block):
        """Display block details"""
        print(f"\n=== BLOCK #{block.index} ===")
        print(f"Previous Hash: {block.previous_hash}")
        print(f"Current Hash:  {block.hash}")
        print(f"Timestamp:     {datetime.fromtimestamp(block.timestamp).strftime('%Y-%m-%d %H:%M:%S')}")
        print(f"Transactions:  {block.transaction_count}")

        for i, tx in enumerate(block.transactions):
            sender = tx.get('sender', 'N/A')
            receiver = tx.get('receiver', 'N/A')
            amount = tx.get('amount', 0)

            print(f"  {i+1}. {sender} -> {receiver} ({amount:.2f})")

        print("=" * 40)

    def display_stats(self, stats: Dict):
        """Display blockchain statistics"""
        print(f"\n=== BLOCKCHAIN STATISTICS ===")
        print(f"Total Blocks:         {stats['total_blocks']}")
        print(f"Total Transactions:    {stats['total_transactions']}")
        print(f"Latest Block Index:   {stats['last_block_index']}")
        print(f"Last Block Hash:      {stats['last_block_hash']}")
        age_seconds = stats.get('blockchain_age_seconds', 0)
        if age_seconds > 0:
            age_minutes = int(age_seconds / 60)
            age_hours = age_minutes / 60
            print(f"Blockchain Age:        {age_hours:.1f} hours ({age_minutes} minutes)")
        print("=" * 40)

    def save_to_file(self, filename: str = 'blockchain_data.json'):
        """Save blockchain data to JSON file"""
        data = {
            'blocks': [block.to_dict() for block in self.blocks],
            'stats': self.get_blockchain_stats()
        }

        with open(filename, 'w') as f:
            json.dump(data, f, indent=2)

        print(f"Blockchain data saved to {filename}")


def main():
    """Main function"""
    print("=" * 50)
    print("BLOCKCHAIN EXPLORER - DATA VISUALIZATION")
    print("=" * 50)
    print("Author: PARTH")
    print(f"Date:   {datetime.now().strftime('%Y-%m-%d')}")
    print("=" * 50)

    # Create blockchain explorer
    explorer = BlockchainExplorer()

    # Create genesis block
    genesis = explorer.create_genesis_block()
    time.sleep(0.5)

    # Add sample blocks with transactions
    sample_transactions = [
        [{'sender': 'Alice', 'receiver': 'Bob', 'amount': 10.5},
        {'sender': 'Bob', 'receiver': 'Charlie', 'amount': 5.25},
        {'sender': 'Charlie', 'receiver': 'Alice', 'amount': 2.75}
    ]
    explorer.add_block(sample_transactions)
    time.sleep(0.3)

    # Add another block
    more_transactions = [
        {'sender': 'Dave', 'receiver': 'Eve', 'amount': 8.0},
        {'sender': 'Eve', 'receiver': 'Frank', 'amount': 15.0}
    ]
    explorer.add_block(more_transactions)
    time.sleep(0.3)

    # Get latest blocks
    latest_blocks = explorer.get_latest_blocks(3)
    print(f"\nDisplaying latest {len(latest_blocks)} blocks...\n")
    for block in latest_blocks:
        explorer.display_block(block)
        time.sleep(0.2)

    # Display statistics
    stats = explorer.get_blockchain_stats()
    explorer.display_stats(stats)

    # Save to file
    explorer.save_to_file('blockchain_export.json')

    print("\n=== EXPLORER COMPLETE ===")
    print(f"Total blocks created: {explorer.block_count}")
    print(f"Total transactions: {stats['total_transactions']}")
    print("Data saved to blockchain_export.json")
    print("=" * 50)

    return 0


if __name__ == "__main__":
    main()
