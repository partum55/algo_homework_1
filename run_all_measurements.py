#!/usr/bin/env python3
"""
Script to run all measurements, collect data, and generate plots
"""

import subprocess
import os
import re
import json
from pathlib import Path
import matplotlib
matplotlib.use('Agg')  # Use non-interactive backend
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
from datetime import datetime

# Set seaborn style for better-looking plots
sns.set_theme(style="whitegrid")

class MeasurementRunner:
    def __init__(self, root_dir):
        self.root_dir = Path(root_dir)
        self.results = {}
        self.output_dir = self.root_dir / "results"
        self.output_dir.mkdir(exist_ok=True)
        
    def compile_and_run(self, source_path, executable_name, work_dir):
        """Compile and run a C++ program, return its output"""
        source_file = work_dir / source_path
        executable = work_dir / executable_name
        
        print(f"\n{'='*60}")
        print(f"Compiling {source_file}...")
        print(f"{'='*60}")
        
        # Compile
        compile_cmd = ["g++", str(source_file), "-o", str(executable), "-std=c++17", "-O2"]
        try:
            result = subprocess.run(compile_cmd, capture_output=True, text=True, check=True)
            print(f"✓ Compilation successful")
        except subprocess.CalledProcessError as e:
            print(f"✗ Compilation failed: {e.stderr}")
            return None
        
        # Run
        print(f"Running {executable_name}...")
        try:
            result = subprocess.run([str(executable)], capture_output=True, text=True, 
                                   cwd=work_dir, timeout=30)
            print(f"✓ Execution completed")
            return result.stdout
        except subprocess.TimeoutExpired:
            print(f"✗ Execution timed out")
            return None
        except Exception as e:
            print(f"✗ Execution failed: {e}")
            return None
    
    def parse_output(self, output):
        """Parse the output from measurement programs"""
        if not output:
            return None
        
        data = {}
        
        # Extract duration
        duration_match = re.search(r'Duration:\s*(\d+)\s*ms', output)
        if duration_match:
            data['duration_ms'] = int(duration_match.group(1))
        
        # Extract total operations
        total_ops_match = re.search(r'Total operations:\s*(\d+)', output)
        if total_ops_match:
            data['total_operations'] = int(total_ops_match.group(1))
        
        # Extract operations per second (handles scientific notation like 3.08928e+06)
        ops_per_sec_match = re.search(r'Operations per second:\s*([\d.eE+\-]+)', output)
        if ops_per_sec_match:
            data['ops_per_second'] = float(ops_per_sec_match.group(1))
        
        # Extract individual operation counts
        op1_match = re.search(r'Operation 1.*?:\s*(\d+)', output)
        op2_match = re.search(r'Operation 2.*?:\s*(\d+)', output)
        op3_match = re.search(r'Operation 3.*?:\s*(\d+)', output)
        
        if op1_match:
            data['op1_count'] = int(op1_match.group(1))
        if op2_match:
            data['op2_count'] = int(op2_match.group(1))
        if op3_match:
            data['op3_count'] = int(op3_match.group(1))
        
        return data
    
    def run_all_measurements(self):
        """Run all measurement programs"""
        measurements = [
            {
                'name': 'Hash Table',
                'source': 'measure.cpp',
                'executable': 'main',
                'work_dir': self.root_dir / 'standart' / 'hash'
            },
            {
                'name': 'Vector',
                'source': 'measure.cpp',
                'executable': 'main',
                'work_dir': self.root_dir / 'standart' / 'vector'
            },
            {
                'name': 'AVL Tree',
                'source': 'measure.cpp',
                'executable': 'main',
                'work_dir': self.root_dir / 'standart' / 'avl'
            },
            {
                'name': 'Optimized',
                'source': 'measure_optimized.cpp',
                'executable': 'main',
                'work_dir': self.root_dir / 'optimized'
            }
        ]
        
        for measurement in measurements:
            output = self.compile_and_run(
                measurement['source'],
                measurement['executable'],
                measurement['work_dir']
            )
            
            if output:
                print(f"\n--- Output for {measurement['name']} ---")
                print(output)
                
                parsed_data = self.parse_output(output)
                if parsed_data:
                    self.results[measurement['name']] = parsed_data
                    print(f"✓ Data collected for {measurement['name']}")
                else:
                    print(f"✗ Failed to parse output for {measurement['name']}")
        
        # Save results to JSON
        results_file = self.output_dir / 'measurement_results.json'
        with open(results_file, 'w') as f:
            json.dump(self.results, f, indent=2)
        print(f"\n✓ Results saved to {results_file}")
        
        return self.results
    
    def generate_plots(self):
        """Generate various plots from collected data"""
        if not self.results:
            print("No results to plot!")
            return
        
        # Convert to DataFrame
        df = pd.DataFrame(self.results).T
        df['implementation'] = df.index
        
        # Create figure with subplots
        fig = plt.figure(figsize=(16, 10))
        
        # 1. Operations per second comparison
        ax1 = plt.subplot(2, 3, 1)
        implementations = df['implementation'].tolist()
        ops_per_sec = df['ops_per_second'].tolist()
        colors = sns.color_palette("husl", len(implementations))
        bars = ax1.bar(implementations, ops_per_sec, color=colors, edgecolor='black', linewidth=1.5)
        ax1.set_ylabel('Operations per Second', fontsize=12, fontweight='bold')
        ax1.set_xlabel('Implementation', fontsize=12, fontweight='bold')
        ax1.set_title('Performance Comparison\n(Higher is Better)', fontsize=14, fontweight='bold')
        ax1.tick_params(axis='x', rotation=45)
        ax1.grid(axis='y', alpha=0.3)
        
        # Add value labels on bars
        for bar in bars:
            height = bar.get_height()
            ax1.text(bar.get_x() + bar.get_width()/2., height,
                    f'{height:.0f}',
                    ha='center', va='bottom', fontsize=10, fontweight='bold')
        
        # 2. Total operations comparison
        ax2 = plt.subplot(2, 3, 2)
        total_ops = df['total_operations'].tolist()
        bars = ax2.bar(implementations, total_ops, color=colors, edgecolor='black', linewidth=1.5)
        ax2.set_ylabel('Total Operations', fontsize=12, fontweight='bold')
        ax2.set_xlabel('Implementation', fontsize=12, fontweight='bold')
        ax2.set_title('Total Operations in 10 seconds', fontsize=14, fontweight='bold')
        ax2.tick_params(axis='x', rotation=45)
        ax2.grid(axis='y', alpha=0.3)
        
        for bar in bars:
            height = bar.get_height()
            ax2.text(bar.get_x() + bar.get_width()/2., height,
                    f'{int(height)}',
                    ha='center', va='bottom', fontsize=10, fontweight='bold')
        
        # 3. Duration comparison
        ax3 = plt.subplot(2, 3, 3)
        durations = df['duration_ms'].tolist()
        bars = ax3.bar(implementations, durations, color=colors, edgecolor='black', linewidth=1.5)
        ax3.set_ylabel('Duration (ms)', fontsize=12, fontweight='bold')
        ax3.set_xlabel('Implementation', fontsize=12, fontweight='bold')
        ax3.set_title('Execution Time', fontsize=14, fontweight='bold')
        ax3.tick_params(axis='x', rotation=45)
        ax3.grid(axis='y', alpha=0.3)
        
        for bar in bars:
            height = bar.get_height()
            ax3.text(bar.get_x() + bar.get_width()/2., height,
                    f'{int(height)}',
                    ha='center', va='bottom', fontsize=10, fontweight='bold')
        
        # 4. Operation breakdown (stacked bar chart)
        ax4 = plt.subplot(2, 3, 4)
        op1_counts = df['op1_count'].tolist()
        op2_counts = df['op2_count'].tolist()
        op3_counts = df['op3_count'].tolist()
        
        x_pos = range(len(implementations))
        ax4.bar(x_pos, op1_counts, label='Op1: Find max group', color='#ff6b6b', edgecolor='black')
        ax4.bar(x_pos, op2_counts, bottom=op1_counts, label='Op2: Change group', color='#4ecdc4', edgecolor='black')
        bottom = [op1_counts[i] + op2_counts[i] for i in range(len(implementations))]
        ax4.bar(x_pos, op3_counts, bottom=bottom, label='Op3: Find best rating', color='#45b7d1', edgecolor='black')
        
        ax4.set_ylabel('Number of Operations', fontsize=12, fontweight='bold')
        ax4.set_xlabel('Implementation', fontsize=12, fontweight='bold')
        ax4.set_title('Operation Breakdown', fontsize=14, fontweight='bold')
        ax4.set_xticks(x_pos)
        ax4.set_xticklabels(implementations, rotation=45)
        ax4.legend(loc='upper left', fontsize=9)
        ax4.grid(axis='y', alpha=0.3)
        
        # 5. Normalized performance (relative to slowest)
        ax5 = plt.subplot(2, 3, 5)
        min_ops_per_sec = min(ops_per_sec)
        normalized_perf = [ops / min_ops_per_sec for ops in ops_per_sec]
        bars = ax5.bar(implementations, normalized_perf, color=colors, edgecolor='black', linewidth=1.5)
        ax5.set_ylabel('Relative Performance', fontsize=12, fontweight='bold')
        ax5.set_xlabel('Implementation', fontsize=12, fontweight='bold')
        ax5.set_title('Speedup vs Slowest Implementation', fontsize=14, fontweight='bold')
        ax5.tick_params(axis='x', rotation=45)
        ax5.axhline(y=1, color='red', linestyle='--', linewidth=2, label='Baseline')
        ax5.grid(axis='y', alpha=0.3)
        ax5.legend()
        
        for bar in bars:
            height = bar.get_height()
            ax5.text(bar.get_x() + bar.get_width()/2., height,
                    f'{height:.2f}x',
                    ha='center', va='bottom', fontsize=10, fontweight='bold')
        
        # 6. Summary table
        ax6 = plt.subplot(2, 3, 6)
        ax6.axis('tight')
        ax6.axis('off')
        
        table_data = []
        for impl in implementations:
            data = self.results[impl]
            table_data.append([
                impl,
                f"{data['ops_per_second']:,.0f}",
                f"{data['total_operations']:,}",
                f"{data['duration_ms']:,} ms"
            ])
        
        table = ax6.table(cellText=table_data,
                         colLabels=['Implementation', 'Ops/sec', 'Total Ops', 'Duration'],
                         cellLoc='center',
                         loc='center',
                         colWidths=[0.3, 0.25, 0.25, 0.2])
        table.auto_set_font_size(False)
        table.set_fontsize(10)
        table.scale(1, 2)
        
        # Style header
        for i in range(4):
            table[(0, i)].set_facecolor('#4a90e2')
            table[(0, i)].set_text_props(weight='bold', color='white')
        
        # Alternate row colors
        for i in range(1, len(table_data) + 1):
            for j in range(4):
                if i % 2 == 0:
                    table[(i, j)].set_facecolor('#f0f0f0')
        
        ax6.set_title('Performance Summary', fontsize=14, fontweight='bold', pad=20)
        
        plt.suptitle('Data Structures Performance Analysis', 
                    fontsize=18, fontweight='bold', y=0.98)
        plt.tight_layout(rect=[0, 0, 1, 0.96])
        
        # Save plot
        plot_file = self.output_dir / 'performance_comparison.png'
        plt.savefig(plot_file, dpi=300, bbox_inches='tight')
        print(f"\n✓ Plot saved to {plot_file}")
        
        # Also save as PDF
        pdf_file = self.output_dir / 'performance_comparison.pdf'
        plt.savefig(pdf_file, bbox_inches='tight')
        print(f"✓ Plot saved to {pdf_file}")
        
        plt.close()

        # Generate individual operation comparison plot
        self._generate_operation_comparison_plot()
    
    def _generate_operation_comparison_plot(self):
        """Generate detailed operation comparison plot"""
        fig, axes = plt.subplots(1, 3, figsize=(15, 5))
        
        implementations = list(self.results.keys())
        colors = sns.color_palette("husl", len(implementations))
        
        operations = [
            ('op1_count', 'Operation 1: Find Group with Most Students'),
            ('op2_count', 'Operation 2: Change Student Group'),
            ('op3_count', 'Operation 3: Find Group with Highest Rating')
        ]
        
        for idx, (op_key, op_title) in enumerate(operations):
            ax = axes[idx]
            op_counts = [self.results[impl][op_key] for impl in implementations]
            bars = ax.bar(implementations, op_counts, color=colors, edgecolor='black', linewidth=1.5)
            ax.set_ylabel('Operations Count', fontsize=11, fontweight='bold')
            ax.set_xlabel('Implementation', fontsize=11, fontweight='bold')
            ax.set_title(op_title, fontsize=12, fontweight='bold')
            ax.tick_params(axis='x', rotation=45)
            ax.grid(axis='y', alpha=0.3)
            
            for bar in bars:
                height = bar.get_height()
                ax.text(bar.get_x() + bar.get_width()/2., height,
                       f'{int(height)}',
                       ha='center', va='bottom', fontsize=9, fontweight='bold')
        
        plt.suptitle('Individual Operations Comparison', fontsize=16, fontweight='bold')
        plt.tight_layout()
        
        plot_file = self.output_dir / 'operations_comparison.png'
        plt.savefig(plot_file, dpi=300, bbox_inches='tight')
        print(f"✓ Operations plot saved to {plot_file}")
        
        pdf_file = self.output_dir / 'operations_comparison.pdf'
        plt.savefig(pdf_file, bbox_inches='tight')
        print(f"✓ Operations plot saved to {pdf_file}")
        
        plt.close()

    def generate_report(self):
        """Generate a text report of the results"""
        if not self.results:
            print("No results to report!")
            return
        
        report_file = self.output_dir / 'performance_report.txt'
        
        with open(report_file, 'w') as f:
            f.write("="*70 + "\n")
            f.write("PERFORMANCE ANALYSIS REPORT\n")
            f.write(f"Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
            f.write("="*70 + "\n\n")
            
            # Find best performer
            best_impl = max(self.results.items(), key=lambda x: x[1]['ops_per_second'])
            worst_impl = min(self.results.items(), key=lambda x: x[1]['ops_per_second'])
            
            f.write("SUMMARY\n")
            f.write("-"*70 + "\n")
            f.write(f"Best Implementation: {best_impl[0]}\n")
            f.write(f"  Operations/second: {best_impl[1]['ops_per_second']:,.2f}\n")
            f.write(f"\nWorst Implementation: {worst_impl[0]}\n")
            f.write(f"  Operations/second: {worst_impl[1]['ops_per_second']:,.2f}\n")
            f.write(f"\nSpeedup: {best_impl[1]['ops_per_second'] / worst_impl[1]['ops_per_second']:.2f}x\n")
            f.write("\n")
            
            f.write("DETAILED RESULTS\n")
            f.write("-"*70 + "\n")
            
            for impl_name, data in sorted(self.results.items(), 
                                         key=lambda x: x[1]['ops_per_second'], 
                                         reverse=True):
                f.write(f"\n{impl_name}:\n")
                f.write(f"  Operations per second: {data['ops_per_second']:,.2f}\n")
                f.write(f"  Total operations: {data['total_operations']:,}\n")
                f.write(f"  Duration: {data['duration_ms']:,} ms\n")
                f.write(f"  Operation 1 count: {data['op1_count']:,}\n")
                f.write(f"  Operation 2 count: {data['op2_count']:,}\n")
                f.write(f"  Operation 3 count: {data['op3_count']:,}\n")

            f.write("\n" + "="*70 + "\n")
        
        print(f"✓ Report saved to {report_file}")
        
        # Print report to console
        with open(report_file, 'r') as f:
            print("\n" + f.read())


def main():
    print("╔═══════════════════════════════════════════════════════════════╗")
    print("║   Data Structures Performance Measurement & Analysis Tool    ║")
    print("╚═══════════════════════════════════════════════════════════════╝")
    
    root_dir = os.path.dirname(os.path.abspath(__file__))
    runner = MeasurementRunner(root_dir)
    
    print("\n[1/3] Running all measurements...")
    results = runner.run_all_measurements()
    
    if results:
        print("\n[2/3] Generating plots...")
        runner.generate_plots()
        
        print("\n[3/3] Generating report...")
        runner.generate_report()
        
        print("\n" + "="*70)
        print("✓ All tasks completed successfully!")
        print(f"✓ Results saved in: {runner.output_dir}")
        print("="*70)
    else:
        print("\n✗ No results collected. Please check for errors above.")


if __name__ == "__main__":
    main()

