# CPTS471 PA3 Report

## Team

Miranda Stoddard

Ace Cassidy

## System Configuration

* CPU: Intel Core i5-8350U
* Clock Rate: 1.70GHz X 8
* Ram: 16 Gb DDR4
* OS: Ubuntu 20.04

## Quality

### Fingerprint lengths

| String          | Length  | Fingerprint |
|-----------------|---------| ----------- |
| Covid_Australia |   8     | GTACCCCA    |
| Covid_Brazil    |   7     | GCGCTCG     |
|Covid_India      |   8     | GTCTTTTC    |
| Covid_USA       |   8     | TGACTTAC    |
| Covid_Wuhan     |   125   | GTACAGTGA...|
| MERS_2012_KF    |   X     |     X       |
| MERS_2014_KY    |   6     | TCCCCC      |
| MERS_2014_USA   |   6     | GTCCCC      |
| SARS_2003_GU    |   2     | TGCCCCG     |
| SARS_2017_MK    |   7     | TCTGCCC     |

### Fingerprint Observations

We can tell from the fingerprint length that Covid_Wuhan is the most similiar to other strains, which makes sense because all of the other Covid strains started from that.
Other strains seem to all be about equally related to at least one other strain, meaning they have around the same fingerprint length.

### Match values

| String          | Covid_Australia | Covid_Brazil | Covid_India | Covid_USA | Covid_Wuhan | MERS_2014_KY | MERS_2014_USA | SARS_2003_GU | SARS_2017_MK |
|-----------------|-----------------|--------------|-------------|-----------|-------------|--------------|---------------|--------------|--------------|
| Covid_Australia |        x        |   29836      |  29797      | 29839     | 29870       | 211          |  214          | 11257        | 11302        |
| Covid_Brazil    |        x        |      X       |  29819      | 29855     | 29864       | 208          |  211          | 11285        | 11330        |
| Covid_India     |        x        |      X       |      X      | 29822     | 29831       | 205          |  208          | 11277        | 11318        |
| Covid_USA       |        x        |      X       |      X      |     X     | 29873       | 211          |  214          | 11276        | 11321        |
| Covid_Wuhan     |        x        |      X       |      X      |     X     |      X      | 211          |  214          | 11285        | 11330        |
| MERS_2014_KY    |        x        |      X       |      X      |     X     |      X      |     X        |  29914        | 141          | 141          |
| MERS_2014_USA   |        x        |      X       |      X      |     X     |      X      |     X        |       X       | 141          | 141          |
| SARS_2003_GU    |        x        |      X       |      X      |     X     |      X      |     X        |       X       |      X       | 29592        |
| SARS_2017_MK    |        x        |      X       |      X      |     X     |      X      |     X        |       X       |      X       |       X      |

### Matrix Observations

Based on the matrix each virus seems most related to other strains of the same virus. Covid seems to be more related to SARS than MERS and MERS seems to be more related
to Covid than SARS although it is not very related to either of them compared to how closely Covid and SARS are related. 

## Construction Performance

### LCA Lengths

