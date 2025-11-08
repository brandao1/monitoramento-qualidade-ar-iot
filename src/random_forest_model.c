#include <string.h>
void add_vectors(double *v1, double *v2, int size, double *result) {
    for(int i = 0; i < size; ++i)
        result[i] = v1[i] + v2[i];
}
void mul_vector_number(double *v1, double num, int size, double *result) {
    for(int i = 0; i < size; ++i)
        result[i] = v1[i] * num;
}
void score(double * input, double * output) {
    double var0[3];
    double var1[3];
    double var2[3];
    double var3[3];
    double var4[3];
    double var5[3];
    double var6[3];
    double var7[3];
    double var8[3];
    double var9[3];
    double var10[3];
    if (input[5] <= -0.05413149110972881) {
        if (input[2] <= -0.04868917725980282) {
            if (input[5] <= -0.0541515126824379) {
                if (input[7] <= -0.17597949504852295) {
                    if (input[3] <= -0.012417882215231657) {
                        memcpy(var10, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                    } else {
                        memcpy(var10, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                    }
                } else {
                    memcpy(var10, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                }
            } else {
                if (input[3] <= 0.025605392642319202) {
                    memcpy(var10, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                } else {
                    if (input[5] <= -0.054151277989149094) {
                        memcpy(var10, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                    } else {
                        memcpy(var10, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                    }
                }
            }
        } else {
            if (input[2] <= -0.04868914373219013) {
                memcpy(var10, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
            } else {
                memcpy(var10, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
            }
        }
    } else {
        if (input[3] <= -0.05927962437272072) {
            if (input[6] <= -0.04432769492268562) {
                memcpy(var10, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
            } else {
                memcpy(var10, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
            }
        } else {
            if (input[6] <= -0.04432762414216995) {
                if (input[4] <= -0.04383289813995361) {
                    if (input[5] <= -0.05412112735211849) {
                        if (input[6] <= -0.0443281214684248) {
                            memcpy(var10, (double[]){0.0, 0.8888888888888888, 0.1111111111111111}, 3 * sizeof(double));
                        } else {
                            memcpy(var10, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                        }
                    } else {
                        if (input[6] <= -0.04432804696261883) {
                            memcpy(var10, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                        } else {
                            memcpy(var10, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                        }
                    }
                } else {
                    if (input[4] <= -0.04383268766105175) {
                        memcpy(var10, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                    } else {
                        if (input[7] <= -0.11343038454651833) {
                            memcpy(var10, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                        } else {
                            memcpy(var10, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                        }
                    }
                }
            } else {
                memcpy(var10, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
            }
        }
    }
    double var11[3];
    if (input[2] <= -0.04868895374238491) {
        if (input[3] <= -0.005899771582335234) {
            memcpy(var11, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
        } else {
            if (input[3] <= 0.10833071544766426) {
                memcpy(var11, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
            } else {
                memcpy(var11, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
            }
        }
    } else {
        if (input[7] <= -0.10993140935897827) {
            if (input[2] <= -0.04868863523006439) {
                if (input[6] <= -0.044328125193715096) {
                    memcpy(var11, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                } else {
                    memcpy(var11, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                }
            } else {
                if (input[7] <= -0.11365259811282158) {
                    if (input[3] <= 1.2732077669352293) {
                        memcpy(var11, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                    } else {
                        memcpy(var11, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                    }
                } else {
                    if (input[5] <= -0.05409257672727108) {
                        memcpy(var11, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                    } else {
                        memcpy(var11, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                    }
                }
            }
        } else {
            memcpy(var11, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
        }
    }
    add_vectors(var10, var11, 3, var9);
    double var12[3];
    if (input[6] <= -0.044328151270747185) {
        if (input[5] <= -0.05415329523384571) {
            if (input[3] <= 0.004068626461958047) {
                memcpy(var12, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
            } else {
                if (input[3] <= 0.09410750865936279) {
                    memcpy(var12, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                } else {
                    memcpy(var12, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                }
            }
        } else {
            if (input[2] <= -0.048691293224692345) {
                memcpy(var12, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
            } else {
                if (input[5] <= -0.054151626303792) {
                    memcpy(var12, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                } else {
                    if (input[2] <= -0.048690831288695335) {
                        if (input[3] <= 0.15950365457683802) {
                            memcpy(var12, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                        } else {
                            memcpy(var12, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                        }
                    } else {
                        memcpy(var12, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                    }
                }
            }
        }
    } else {
        if (input[7] <= -0.10993140935897827) {
            if (input[7] <= -0.13648172467947006) {
                if (input[4] <= -0.04383297078311443) {
                    memcpy(var12, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                } else {
                    memcpy(var12, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                }
            } else {
                if (input[5] <= -0.05412885546684265) {
                    if (input[2] <= -0.04868866875767708) {
                        memcpy(var12, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                    } else {
                        memcpy(var12, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                    }
                } else {
                    memcpy(var12, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                }
            }
        } else {
            memcpy(var12, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
        }
    }
    add_vectors(var9, var12, 3, var8);
    double var13[3];
    if (input[2] <= -0.048688970506191254) {
        if (input[6] <= -0.04432827793061733) {
            if (input[7] <= -0.16116974502801895) {
                if (input[5] <= -0.05415329523384571) {
                    if (input[7] <= -0.17597949504852295) {
                        if (input[3] <= -0.013595340307801962) {
                            memcpy(var13, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                        } else {
                            memcpy(var13, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                        }
                    } else {
                        memcpy(var13, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                    }
                } else {
                    if (input[5] <= -0.05415165051817894) {
                        memcpy(var13, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                    } else {
                        if (input[5] <= -0.054151611402630806) {
                            memcpy(var13, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                        } else {
                            memcpy(var13, (double[]){0.96, 0.02, 0.02}, 3 * sizeof(double));
                        }
                    }
                }
            } else {
                if (input[5] <= -0.05415081977844238) {
                    memcpy(var13, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                } else {
                    memcpy(var13, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                }
            }
        } else {
            if (input[4] <= -0.043833041563630104) {
                if (input[5] <= -0.054146965965628624) {
                    memcpy(var13, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                } else {
                    if (input[2] <= -0.048690831288695335) {
                        memcpy(var13, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                    } else {
                        memcpy(var13, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                    }
                }
            } else {
                memcpy(var13, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
            }
        }
    } else {
        if (input[4] <= -0.04383258894085884) {
            if (input[7] <= -0.12836024165153503) {
                if (input[4] <= -0.04383290372788906) {
                    memcpy(var13, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                } else {
                    memcpy(var13, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                }
            } else {
                memcpy(var13, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
            }
        } else {
            memcpy(var13, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
        }
    }
    add_vectors(var8, var13, 3, var7);
    double var14[3];
    if (input[7] <= -0.1377696916460991) {
        if (input[4] <= -0.043833041563630104) {
            if (input[5] <= -0.054146965965628624) {
                if (input[2] <= -0.04869128577411175) {
                    if (input[7] <= -0.17598411440849304) {
                        if (input[3] <= -0.01117116678506136) {
                            memcpy(var14, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                        } else {
                            memcpy(var14, (double[]){0.0, 0.8888888888888888, 0.1111111111111111}, 3 * sizeof(double));
                        }
                    } else {
                        memcpy(var14, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                    }
                } else {
                    if (input[5] <= -0.05415150709450245) {
                        if (input[3] <= 0.05255838669836521) {
                            memcpy(var14, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                        } else {
                            memcpy(var14, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                        }
                    } else {
                        if (input[3] <= 0.05703143123537302) {
                            memcpy(var14, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                        } else {
                            memcpy(var14, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                        }
                    }
                }
            } else {
                if (input[3] <= 0.4070000480860472) {
                    memcpy(var14, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                } else {
                    memcpy(var14, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                }
            }
        } else {
            memcpy(var14, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
        }
    } else {
        if (input[6] <= -0.0443276297301054) {
            if (input[7] <= -0.13650751113891602) {
                if (input[2] <= -0.04868912883102894) {
                    memcpy(var14, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                } else {
                    if (input[7] <= -0.13686513900756836) {
                        memcpy(var14, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                    } else {
                        if (input[5] <= -0.054131776094436646) {
                            memcpy(var14, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                        } else {
                            memcpy(var14, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                        }
                    }
                }
            } else {
                if (input[7] <= -0.1289486214518547) {
                    memcpy(var14, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                } else {
                    if (input[6] <= -0.04432804696261883) {
                        memcpy(var14, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                    } else {
                        if (input[3] <= 0.41497223265469074) {
                            memcpy(var14, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                        } else {
                            memcpy(var14, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                        }
                    }
                }
            }
        } else {
            memcpy(var14, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
        }
    }
    add_vectors(var7, var14, 3, var6);
    double var15[3];
    if (input[7] <= -0.13680382072925568) {
        if (input[2] <= -0.048691218718886375) {
            if (input[5] <= -0.054150842130184174) {
                if (input[7] <= -0.1632620245218277) {
                    if (input[3] <= 0.004068626461958047) {
                        memcpy(var15, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                    } else {
                        if (input[3] <= 0.10833071544766426) {
                            memcpy(var15, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                        } else {
                            memcpy(var15, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                        }
                    }
                } else {
                    if (input[7] <= -0.16312656551599503) {
                        memcpy(var15, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                    } else {
                        if (input[4] <= -0.0438330564647913) {
                            memcpy(var15, (double[]){0.6666666666666666, 0.3333333333333333, 0.0}, 3 * sizeof(double));
                        } else {
                            memcpy(var15, (double[]){0.9607843137254902, 0.0, 0.0392156862745098}, 3 * sizeof(double));
                        }
                    }
                }
            } else {
                if (input[3] <= 0.041242172941565514) {
                    memcpy(var15, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                } else {
                    memcpy(var15, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                }
            }
        } else {
            if (input[3] <= 0.42426720075309277) {
                memcpy(var15, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
            } else {
                memcpy(var15, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
            }
        }
    } else {
        if (input[4] <= -0.04383261688053608) {
            if (input[5] <= -0.05413155257701874) {
                memcpy(var15, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
            } else {
                if (input[4] <= -0.04383268766105175) {
                    if (input[5] <= -0.05412885546684265) {
                        if (input[6] <= -0.044328125193715096) {
                            memcpy(var15, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                        } else {
                            memcpy(var15, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                        }
                    } else {
                        memcpy(var15, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                    }
                } else {
                    if (input[6] <= -0.044327711686491966) {
                        memcpy(var15, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                    } else {
                        memcpy(var15, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                    }
                }
            }
        } else {
            memcpy(var15, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
        }
    }
    add_vectors(var6, var15, 3, var5);
    double var16[3];
    if (input[3] <= -0.059301065281033516) {
        if (input[7] <= -0.11245051398873329) {
            if (input[4] <= -0.043832942843437195) {
                memcpy(var16, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
            } else {
                memcpy(var16, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
            }
        } else {
            memcpy(var16, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
        }
    } else {
        if (input[6] <= -0.044328147545456886) {
            if (input[6] <= -0.044328153133392334) {
                if (input[5] <= -0.05415165051817894) {
                    if (input[3] <= -0.005899771582335234) {
                        memcpy(var16, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                    } else {
                        memcpy(var16, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                    }
                } else {
                    if (input[7] <= -0.16312656551599503) {
                        if (input[3] <= -0.014497955329716206) {
                            memcpy(var16, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                        } else {
                            memcpy(var16, (double[]){0.0, 0.2, 0.8}, 3 * sizeof(double));
                        }
                    } else {
                        if (input[5] <= -0.054150866344571114) {
                            memcpy(var16, (double[]){0.9444444444444444, 0.0, 0.05555555555555555}, 3 * sizeof(double));
                        } else {
                            memcpy(var16, (double[]){0.9927007299270073, 0.0, 0.0072992700729927005}, 3 * sizeof(double));
                        }
                    }
                }
            } else {
                if (input[5] <= -0.054131776094436646) {
                    memcpy(var16, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                } else {
                    memcpy(var16, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                }
            }
        } else {
            if (input[4] <= -0.04383259080350399) {
                if (input[4] <= -0.04383269511163235) {
                    memcpy(var16, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                } else {
                    if (input[4] <= -0.043832674622535706) {
                        memcpy(var16, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                    } else {
                        memcpy(var16, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                    }
                }
            } else {
                memcpy(var16, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
            }
        }
    }
    add_vectors(var5, var16, 3, var4);
    double var17[3];
    if (input[4] <= -0.043832967057824135) {
        if (input[6] <= -0.044328147545456886) {
            if (input[6] <= -0.04432827793061733) {
                if (input[7] <= -0.16115368157625198) {
                    if (input[7] <= -0.16359755396842957) {
                        if (input[5] <= -0.05415329523384571) {
                            memcpy(var17, (double[]){0.9487179487179488, 0.03846153846153847, 0.012820512820512822}, 3 * sizeof(double));
                        } else {
                            memcpy(var17, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                        }
                    } else {
                        if (input[7] <= -0.16356374323368073) {
                            memcpy(var17, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                        } else {
                            memcpy(var17, (double[]){0.9, 0.075, 0.025}, 3 * sizeof(double));
                        }
                    }
                } else {
                    if (input[3] <= 0.041242172941565514) {
                        memcpy(var17, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                    } else {
                        memcpy(var17, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                    }
                }
            } else {
                if (input[5] <= -0.0541318841278553) {
                    if (input[6] <= -0.04432825930416584) {
                        if (input[2] <= -0.048690857365727425) {
                            memcpy(var17, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                        } else {
                            memcpy(var17, (double[]){0.5, 0.0, 0.5}, 3 * sizeof(double));
                        }
                    } else {
                        memcpy(var17, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                    }
                } else {
                    if (input[7] <= -0.13671394437551498) {
                        memcpy(var17, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                    } else {
                        memcpy(var17, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                    }
                }
            }
        } else {
            memcpy(var17, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
        }
    } else {
        if (input[4] <= -0.04383261315524578) {
            if (input[7] <= -0.12879931181669235) {
                if (input[5] <= -0.05412112735211849) {
                    if (input[4] <= -0.04383295215666294) {
                        if (input[2] <= -0.048688678070902824) {
                            memcpy(var17, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                        } else {
                            memcpy(var17, (double[]){0.0, 0.6666666666666666, 0.3333333333333333}, 3 * sizeof(double));
                        }
                    } else {
                        memcpy(var17, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                    }
                } else {
                    memcpy(var17, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                }
            } else {
                if (input[7] <= -0.11392172798514366) {
                    memcpy(var17, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                } else {
                    if (input[7] <= -0.11343038454651833) {
                        memcpy(var17, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                    } else {
                        memcpy(var17, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                    }
                }
            }
        } else {
            memcpy(var17, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
        }
    }
    add_vectors(var4, var17, 3, var3);
    double var18[3];
    if (input[6] <= -0.044328147545456886) {
        if (input[3] <= 0.004068626461958047) {
            memcpy(var18, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
        } else {
            if (input[6] <= -0.04432828165590763) {
                if (input[3] <= 0.09410750865936279) {
                    memcpy(var18, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                } else {
                    memcpy(var18, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                }
            } else {
                if (input[7] <= -0.16337116807699203) {
                    memcpy(var18, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                } else {
                    if (input[4] <= -0.0438330564647913) {
                        if (input[5] <= -0.054151501506567) {
                            memcpy(var18, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                        } else {
                            memcpy(var18, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                        }
                    } else {
                        memcpy(var18, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                    }
                }
            }
        }
    } else {
        if (input[3] <= -0.05927962437272072) {
            if (input[5] <= -0.05408729799091816) {
                memcpy(var18, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
            } else {
                memcpy(var18, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
            }
        } else {
            if (input[2] <= -0.04868223890662193) {
                if (input[4] <= -0.0438326932489872) {
                    memcpy(var18, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                } else {
                    if (input[6] <= -0.044327711686491966) {
                        memcpy(var18, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                    } else {
                        memcpy(var18, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                    }
                }
            } else {
                memcpy(var18, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
            }
        }
    }
    add_vectors(var3, var18, 3, var2);
    double var19[3];
    if (input[5] <= -0.05413149110972881) {
        if (input[4] <= -0.04383297823369503) {
            if (input[7] <= -0.1611456423997879) {
                if (input[3] <= 0.004068626461958047) {
                    memcpy(var19, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
                } else {
                    if (input[3] <= 0.10890922322869301) {
                        memcpy(var19, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                    } else {
                        memcpy(var19, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                    }
                }
            } else {
                memcpy(var19, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
            }
        } else {
            if (input[3] <= 0.19946744479238987) {
                memcpy(var19, (double[]){1.0, 0.0, 0.0}, 3 * sizeof(double));
            } else {
                memcpy(var19, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
            }
        }
    } else {
        if (input[2] <= -0.048681823536753654) {
            if (input[4] <= -0.04383295215666294) {
                if (input[4] <= -0.04383295588195324) {
                    memcpy(var19, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                } else {
                    if (input[3] <= 5.391188623383641) {
                        memcpy(var19, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                    } else {
                        memcpy(var19, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                    }
                }
            } else {
                if (input[7] <= -0.11365259811282158) {
                    if (input[2] <= -0.048687467351555824) {
                        if (input[6] <= -0.044328050687909126) {
                            memcpy(var19, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                        } else {
                            memcpy(var19, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                        }
                    } else {
                        memcpy(var19, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                    }
                } else {
                    if (input[4] <= -0.04383268393576145) {
                        memcpy(var19, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
                    } else {
                        memcpy(var19, (double[]){0.0, 1.0, 0.0}, 3 * sizeof(double));
                    }
                }
            }
        } else {
            memcpy(var19, (double[]){0.0, 0.0, 1.0}, 3 * sizeof(double));
        }
    }
    add_vectors(var2, var19, 3, var1);
    mul_vector_number(var1, 0.1, 3, var0);
    memcpy(output, var0, 3 * sizeof(double));
}
